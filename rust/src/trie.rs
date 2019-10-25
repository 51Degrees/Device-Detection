extern crate libc;

use std::ffi::{CString, CStr};
use crate::common::*;
use crate::values::PropertyValue;
use std::alloc::{alloc, Layout};
use std::sync::atomic::{AtomicPtr, Ordering};
use crate::trie_c::*;

pub struct TrieDeviceDetector {
    provider: AtomicPtr<Provider>,
    indexes: PropertyIndexes,
}

type TrieDeviceIndex = ::std::os::raw::c_int;
type DataSet = *mut Dataset;

pub struct TrieDeviceMatch<'detector> {
    indexes: &'detector PropertyIndexes,
    dataset: DataSet,
    device_offset: TrieDeviceIndex,
}

impl<'detector> DeviceMatch for TrieDeviceMatch<'detector> {
    fn get_property(&self, property_type: PropertyName) -> Option<PropertyValue> {
        let str = unsafe {
            let pointer = get_value(self.dataset, self.device_offset, self.indexes[usize::from(&property_type)]);

            CStr::from_ptr(pointer).to_str()
        };

        match str {
            Ok(str) => {
                PropertyValue::new(&property_type, str)
            }
            Err(_) => None
        }
    }
}

impl TrieDeviceDetector {
    pub fn new(file_location: &str, properties: Vec<PropertyName>) -> Result<TrieDeviceDetector, TrieInitStatus> {
        let provider = unsafe {
            alloc(Layout::new::<Provider>())
        } as *mut Provider;

        let file_name = CString::new(file_location).expect("CString::new failed");
        let file_name_ptr = file_name.into_raw();

        let mut property_select = String::new();

        for property in &properties {
            property_select.push_str(property.as_str());
            property_select.push_str(",");
        }

        let property_select = CString::new(property_select).expect("CString::new failed");

        let status = unsafe {
            init_provider_with_property_string(file_name_ptr, provider, property_select.into_raw())
        };

        let dataset = unsafe { (*(*provider).active).dataSet };

        let mut indexes: PropertyIndexes = PROPERTY_INDEXES_EMPTY;

        for property in &properties {
            let c_string = property.as_cstring();

            indexes[usize::from(property)] = unsafe { get_property_index(dataset, c_string.as_ptr()) };
        }

        if status == TrieInitStatus::Success {
            Ok(TrieDeviceDetector {
                provider: AtomicPtr::new(provider),
                indexes,
            })
        } else {
            Err(status)
        }
    }
}

impl<'detector> DeviceDetector<'detector, TrieDeviceMatch<'detector>> for TrieDeviceDetector {
    fn match_by_user_agent(&'detector self, user_agent: &String) -> Option<TrieDeviceMatch<'detector>> {
        let user_agent = CString::new(user_agent.as_bytes()).expect("CString::new failed");

        let dataset = unsafe { (*(*self.provider.load(Ordering::Relaxed)).active).dataSet };

        let device_offset = unsafe {
            get_device_offset(dataset, user_agent.as_ptr())
        };

        Some(TrieDeviceMatch {
            indexes: &self.indexes,
            dataset,
            device_offset,
        })
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use test::Bencher;
    use crate::values::PropertyValue;
    use crate::values::device_type::DeviceType;

    #[test]
    fn trie_init() {
        let detector = TrieDeviceDetector::new("/Users/pholley/Downloads/51Degrees-EnterpriseV3.4.trie", vec![PropertyName::IsSmartPhone, PropertyName::DeviceType, PropertyName::BrowserVersion, PropertyName::PlatformVersion]).ok().unwrap();

        let ua = "Mozilla/5.0 (iPhone; CPU iPhone OS 10_3_2 like Mac OS X) AppleWebKit/603.2.4 (KHTML, like Gecko) FxiOS/7.5b3349 Mobile/14F89 Safari/603.2.4".to_string();

        let matched = detector.match_by_user_agent(&ua).unwrap();

        assert_eq!(matched.get_property(PropertyName::IsSmartPhone).unwrap(), PropertyValue::IsSmartPhone(true));
        assert_eq!(matched.get_property(PropertyName::DeviceType).unwrap(), PropertyValue::DeviceType(DeviceType::SmartPhone));
        assert_eq!(matched.get_property(PropertyName::BrowserVersion).unwrap(), PropertyValue::BrowserVersion("7.5"));
        assert_eq!(matched.get_property(PropertyName::PlatformVersion).unwrap(), PropertyValue::PlatformVersion("10.3.2"));
        //assert_eq!(matched.get_property(PropertyName::BrowserName).unwrap(), PropertyValue::BrowserName(BrowserName::Safari));
    }

    #[bench]
    fn trie_bench(b: &mut Bencher) {
        let detector = TrieDeviceDetector::new("/Users/pholley/Downloads/51Degrees-EnterpriseV3.4.trie", vec![PropertyName::IsSmartPhone, PropertyName::DeviceType]).ok().unwrap();

        // assuming that this will exist in memory already as a result of an HTTP request, so no need to allocate per iteration
        let ua = "Mozilla/5.0 (iPhone; CPU iPhone OS 10_3_2 like Mac OS X) AppleWebKit/603.2.4 (KHTML, like Gecko) FxiOS/7.5b3349 Mobile/14F89 Safari/603.2.4".to_string();

        b.iter(|| {
            detector.match_by_user_agent(&ua)
        });
    }
}
