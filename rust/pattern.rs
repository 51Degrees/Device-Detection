extern crate libc;

use std::mem;
use std::ffi::CString;
use crate::pattern_c::*;

#[derive(PartialEq)]
pub enum PatternInitStatus {
    Success,
    InsufficientMemory,
    CorruptData,
    IncorrectVersion,
    FileNotFound,
    NotSet,
    PointerOutOfBounds,
    NullPointer,
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
    fn new(file_location: &str, properties: Vec<DeviceProperties>) -> Result<DeviceDetector, PatternInitStatus> {
        let provider: *mut fiftyoneDegreesProvider = std::ptr::null_mut();

        let file_name = CString::new(file_location).expect("CString::new failed");
        let file_name_ptr = file_name.into_raw();

        let properties = CString::new("IsMobile,DeviceType").expect("CString::new failed");
        let properties_ptr = properties.into_raw();

        let status_code = unsafe {
            fiftyoneDegreesInitProviderWithPropertyString(file_name_ptr, provider, properties_ptr,0, 0)
        };

        println!("{} ", status_code);

        let status = match status_code {
            e_fiftyoneDegrees_DataSetInitStatus_DATA_SET_INIT_STATUS_SUCCESS => PatternInitStatus::Success,
            e_fiftyoneDegrees_DataSetInitStatus_DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY => PatternInitStatus::InsufficientMemory,
            e_fiftyoneDegrees_DataSetInitStatus_DATA_SET_INIT_STATUS_CORRUPT_DATA => PatternInitStatus::CorruptData,
            e_fiftyoneDegrees_DataSetInitStatus_DATA_SET_INIT_STATUS_INCORRECT_VERSION => PatternInitStatus::IncorrectVersion,
            e_fiftyoneDegrees_DataSetInitStatus_DATA_SET_INIT_STATUS_FILE_NOT_FOUND => PatternInitStatus::FileNotFound,
            e_fiftyoneDegrees_DataSetInitStatus_DATA_SET_INIT_STATUS_NOT_SET => PatternInitStatus::NotSet,
            e_fiftyoneDegrees_DataSetInitStatus_DATA_SET_INIT_STATUS_POINTER_OUT_OF_BOUNDS => PatternInitStatus::PointerOutOfBounds,
            e_fiftyoneDegrees_DataSetInitStatus_DATA_SET_INIT_STATUS_NULL_POINTER => PatternInitStatus::NullPointer,
            _ => PatternInitStatus::Unexpected
        };

        if status == PatternInitStatus::Success {
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
    fn pattern_init() {
        let detector = DeviceDetector::new("../data/51Degrees-LiteV3.2.dat", vec![DeviceProperties::IsMobile,DeviceProperties::DeviceType]);

        assert_eq!(detector.is_ok(), true);
    }
}