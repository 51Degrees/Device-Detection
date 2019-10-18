extern crate libc;

use std::mem;
use std::ffi::CString;
use crate::trie_c::*;

#[derive(PartialEq)]
pub enum InitStatus {
    Success,
    InsufficientMemory,
    CorruptData,
    IncorrectVersion,
    FileNotFound,
    NotSet,
    PointerOutOfBounds,
    NullPointer,
    TooManyOpenFiles,
    Unexpected
}

pub enum DeviceProperties {
    IsMobile,
    DeviceType
}

pub struct DeviceDetector{
    provider: *mut fiftyoneDegreesProvider
}

impl DeviceDetector {
    pub fn new(file_location: &str, properties: Vec<DeviceProperties>) -> Result<DeviceDetector, InitStatus> {
        let provider: *mut fiftyoneDegreesProvider = std::ptr::null_mut();

        let file_name = CString::new(file_location).expect("CString::new failed");
        let file_name_ptr = file_name.into_raw();

        let properties = CString::new("IsMobile,DeviceType").expect("CString::new failed");
        let properties_ptr = properties.into_raw();

        let status_code = unsafe {
            fiftyoneDegreesInitProviderWithPropertyString(file_name_ptr, provider, properties_ptr)
        };

        let status = match status_code {
            e_fiftyoneDegreesDataSetInitStatus_DATA_SET_INIT_STATUS_SUCCESS => InitStatus::Success,
            e_fiftyoneDegreesDataSetInitStatus_DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY => InitStatus::InsufficientMemory,
            e_fiftyoneDegreesDataSetInitStatus_DATA_SET_INIT_STATUS_CORRUPT_DATA => InitStatus::CorruptData,
            e_fiftyoneDegreesDataSetInitStatus_DATA_SET_INIT_STATUS_INCORRECT_VERSION => InitStatus::IncorrectVersion,
            e_fiftyoneDegreesDataSetInitStatus_DATA_SET_INIT_STATUS_FILE_NOT_FOUND => InitStatus::FileNotFound,
            e_fiftyoneDegreesDataSetInitStatus_DATA_SET_INIT_STATUS_NOT_SET => InitStatus::NotSet,
            e_fiftyoneDegreesDataSetInitStatus_DATA_SET_INIT_STATUS_POINTER_OUT_OF_BOUNDS => InitStatus::PointerOutOfBounds,
            e_fiftyoneDegreesDataSetInitStatus_DATA_SET_INIT_STATUS_NULL_POINTER => InitStatus::NullPointer,
            e_fiftyoneDegreesDataSetInitStatus_DATA_SET_INIT_STATUS_TOO_MANY_OPEN_FILES => InitStatus::TooManyOpenFiles,
            _ => InitStatus::Unexpected
        };

        if status == InitStatus::Success {
            Ok(DeviceDetector{
                provider
            })
        }else{
            Err(status)
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn trie_init() {
        let detector = DeviceDetector::new("/tmp/test", vec![DeviceProperties::IsMobile,DeviceProperties::DeviceType]);

        assert_eq!(detector.is_ok(), true);
    }
}