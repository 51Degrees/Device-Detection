extern crate libc;

use std::mem;
use std::ffi::{CString, CStr};
use crate::trie_c::*;
use std::mem::MaybeUninit;
use crate::common::*;
use std::str::Utf8Error;
use crate::values::PropertyValue;
use std::alloc::{alloc, Layout};

#[derive(PartialEq)]
pub enum TrieInitStatus {
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

pub struct TrieDeviceDetector{
    provider: Box<fiftyoneDegreesProvider>,
    indexes: PropertyIndexes
}

pub struct TriePropertyAccessor {
    dataset: *mut fiftyoneDegreesDataSet
}

type TrieDeviceIndex = ::std::os::raw::c_int;
type DataSet = *mut fiftyoneDegreesDataSet;

pub struct TrieDeviceMatch<'detector> {
    indexes: &'detector PropertyIndexes,
    dataset: DataSet,
    device_offset: TrieDeviceIndex
}

impl<'detector> DeviceMatch for TrieDeviceMatch<'detector>{
    fn get_property(&self, property_type: PropertyName) -> Option<PropertyValue> {
        let str = unsafe {
            let pointer = fiftyoneDegreesGetValue(self.dataset, self.device_offset, self.indexes[usize::from(&property_type)]);

            CStr::from_ptr(pointer).to_str()
        };

        match str {
            Ok(str) => {
                PropertyValue::new(&property_type, str)
            },
            Err(err) => None
        }
    }
}

impl TrieDeviceDetector {
    pub fn new(file_location: &str, properties: Vec<PropertyName>) -> Result<TrieDeviceDetector, TrieInitStatus> {
        let provider = unsafe {
            alloc(Layout::new::<fiftyoneDegreesProvider>())
        } as *mut fiftyoneDegreesProvider;

        let file_name = CString::new(file_location).expect("CString::new failed");
        let file_name_ptr = file_name.into_raw();

        let mut property_select = String::new();

        for property in &properties {
            property_select.push_str(property.as_str());
            property_select.push_str(",");
        }

        let property_select = CString::new(property_select).expect("CString::new failed");

        let status_code = unsafe {
            fiftyoneDegreesInitProviderWithPropertyString(file_name_ptr, provider, property_select.into_raw())
        };

        let status = match status_code {
            e_fiftyoneDegreesDataSetInitStatus_DATA_SET_INIT_STATUS_SUCCESS => TrieInitStatus::Success,
            e_fiftyoneDegreesDataSetInitStatus_DATA_SET_INIT_STATUS_INSUFFICIENT_MEMORY => TrieInitStatus::InsufficientMemory,
            e_fiftyoneDegreesDataSetInitStatus_DATA_SET_INIT_STATUS_CORRUPT_DATA => TrieInitStatus::CorruptData,
            e_fiftyoneDegreesDataSetInitStatus_DATA_SET_INIT_STATUS_INCORRECT_VERSION => TrieInitStatus::IncorrectVersion,
            e_fiftyoneDegreesDataSetInitStatus_DATA_SET_INIT_STATUS_FILE_NOT_FOUND => TrieInitStatus::FileNotFound,
            e_fiftyoneDegreesDataSetInitStatus_DATA_SET_INIT_STATUS_NOT_SET => TrieInitStatus::NotSet,
            e_fiftyoneDegreesDataSetInitStatus_DATA_SET_INIT_STATUS_POINTER_OUT_OF_BOUNDS => TrieInitStatus::PointerOutOfBounds,
            e_fiftyoneDegreesDataSetInitStatus_DATA_SET_INIT_STATUS_NULL_POINTER => TrieInitStatus::NullPointer,
            e_fiftyoneDegreesDataSetInitStatus_DATA_SET_INIT_STATUS_TOO_MANY_OPEN_FILES => TrieInitStatus::TooManyOpenFiles,
            _ => TrieInitStatus::Unexpected
        };

        let dataset = unsafe { (*(*provider).active).dataSet };

        let mut indexes:PropertyIndexes = PropertyIndexesEmpty;

        for property in &properties {
            let c_string = property.as_cstring();

            indexes[usize::from(property)] = unsafe { fiftyoneDegreesGetPropertyIndex(dataset,c_string.as_ptr()) };
        }

        if status == TrieInitStatus::Success {
            Ok(TrieDeviceDetector{
                provider: unsafe { Box::from_raw(provider) },
                indexes
            })
        }else{
            Err(status)
        }
    }
}

impl<'detector> DeviceDetector<'detector, TrieDeviceMatch<'detector>> for TrieDeviceDetector{
    fn match_by_user_agent(&'detector self, user_agent: &str) -> Option<TrieDeviceMatch<'detector>> {
        let user_agent = CString::new(user_agent).expect("CString::new failed");

        let dataset = unsafe { (*self.provider.active).dataSet };

        let device_offset = unsafe {
            fiftyoneDegreesGetDeviceOffset(dataset, user_agent.as_ptr())
        };

        Some(TrieDeviceMatch {
            indexes: &self.indexes,
            dataset,
            device_offset
        })
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use test::Bencher;
    use crate::values::PropertyValue;
    use crate::values::device_type::DeviceType;
    use crate::values::browser_name::BrowserName;

    #[test]
    fn trie_init() {
        let mut detector = TrieDeviceDetector::new("/Users/pholley/Downloads/51Degrees-EnterpriseV3.4.trie", vec![PropertyName::IsSmartPhone, PropertyName::DeviceType, PropertyName::BrowserVersion, PropertyName::PlatformVersion]).ok().unwrap();

        let matched = detector.match_by_user_agent("Mozilla/5.0 (iPhone; CPU iPhone OS 10_3_2 like Mac OS X) AppleWebKit/603.2.4 (KHTML, like Gecko) FxiOS/7.5b3349 Mobile/14F89 Safari/603.2.4").unwrap();

        assert_eq!(matched.get_property(PropertyName::IsSmartPhone).unwrap(), PropertyValue::IsSmartPhone(true));
        assert_eq!(matched.get_property(PropertyName::DeviceType).unwrap(), PropertyValue::DeviceType(DeviceType::SmartPhone));
        assert_eq!(matched.get_property(PropertyName::BrowserVersion).unwrap(), PropertyValue::BrowserVersion("7.5"));
        assert_eq!(matched.get_property(PropertyName::PlatformVersion).unwrap(), PropertyValue::PlatformVersion("10.3.2"));
        //assert_eq!(matched.get_property(PropertyName::BrowserName).unwrap(), PropertyValue::BrowserName(BrowserName::Safari));
    }

    #[bench]
    fn trie_bench(b: &mut Bencher) {
        let mut detector = TrieDeviceDetector::new("/Users/pholley/Downloads/51Degrees-EnterpriseV3.4.trie", vec![PropertyName::IsSmartPhone, PropertyName::DeviceType]).ok().unwrap();

        b.iter(|| {
            let matched = detector.match_by_user_agent("Mozilla/5.0 (iPhone; CPU iPhone OS 10_3_2 like Mac OS X) AppleWebKit/603.2.4 (KHTML, like Gecko) FxiOS/7.5b3349 Mobile/14F89 Safari/603.2.4").unwrap();

            assert_eq!(matched.get_property(PropertyName::IsSmartPhone).unwrap(), PropertyValue::IsSmartPhone(true));
        });
    }
}