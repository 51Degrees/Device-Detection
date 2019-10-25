extern crate libc;

use std::ffi::{CString, CStr};
use crate::common::{PropertyIndexes, PropertyName, DeviceMatch, PROPERTY_INDEXES_EMPTY, DeviceDetector};
use crate::values::PropertyValue;
use crate::c::pattern::*;
use std::cell::{RefCell};
use std::mem::MaybeUninit;

pub struct PatternDeviceDetector{
    provider: RefCell<Box<MaybeUninit<Provider>>>,
    indexes: PropertyIndexes
}

pub enum PatternDeviceDetectorError {
    InitStatus(PatternInitStatus),
    InvalidProperty
}


impl PatternDeviceDetector {
    pub fn new(file_location: &str, properties: Vec<PropertyName>) -> Result<PatternDeviceDetector, PatternDeviceDetectorError> {
        let provider:MaybeUninit<Provider> = MaybeUninit::uninit();

        let mut boxed = Box::new(provider);

        let file_name = CString::new(file_location).expect("CString::new failed");

        let mut property_select = String::new();

        for property in &properties {
            property_select.push_str(property.as_str());
            property_select.push_str(",");
        }

        let property_select = CString::new(property_select).expect("CString::new failed");

        let status = unsafe {
            init_provider_with_property_String(file_name.into_raw(), boxed.as_mut_ptr(), property_select.into_raw(), 1, 0)
        };

        if status == PatternInitStatus::Success {
            let dataset = unsafe { (*(*boxed.as_ptr()).activePool).dataSet };

            let mut indexes:PropertyIndexes = PROPERTY_INDEXES_EMPTY;

            for property in &properties {
                let c_string = property.as_cstring();

                let index = unsafe { get_required_property_index(dataset,c_string.as_ptr()) };

                if index == -1 {
                    return Err(PatternDeviceDetectorError::InvalidProperty)
                }

                indexes[usize::from(property)] = index;
            }

            Ok(PatternDeviceDetector{
                provider: RefCell::new(boxed),
                indexes
            })
        }else{
            Err(PatternDeviceDetectorError::InitStatus(status))
        }
    }
}

pub struct PatternDeviceMatch<'detector> {
    indexes: &'detector PropertyIndexes,
    workset: *mut Workset
}

impl<'detector> DeviceMatch for PatternDeviceMatch<'detector>{
    fn get_property(&self, property_type: PropertyName) -> Option<PropertyValue> {
        let str = unsafe {
            set_values(self.workset, self.indexes[usize::from(&property_type)]);

            let vals = (*self.workset).values;

            let val = *vals.offset(0);

            let pointer = get_value_name((*self.workset).dataSet, val);

            CStr::from_ptr(pointer).to_str()
        };

        match str {
            Ok(str) => {
                PropertyValue::new(&property_type, str)
            },
            Err(_) => None
        }
    }
}

impl<'detector> Drop for PatternDeviceMatch<'detector> {
    fn drop(&mut self) {
        unsafe {
            workset_release(self.workset);
        }
    }
}


impl<'detector> DeviceDetector<'detector, PatternDeviceMatch<'detector>> for PatternDeviceDetector{
    fn match_by_user_agent(&'detector self, user_agent: &String) -> Option<PatternDeviceMatch<'detector>> {
        let pointer = unsafe {
            workset_get(self.provider.borrow_mut().as_mut_ptr())
        };

        let user_agent = CString::new(user_agent.as_bytes()).expect("CString::new failed");

        unsafe {
            do_match(pointer, user_agent.as_ptr());

            if (*pointer).method == PatternMatchMethod::None {
                None
            } else {
                Some(PatternDeviceMatch {
                    indexes: &self.indexes,
                    workset: pointer
                })
            }
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use test::Bencher;
    use crate::values::PropertyValue;

    #[test]
    #[cfg(all(feature = "browser-name-enum", feature = "platform-name-enum"))]
    fn pattern_init() {
        let detector = PatternDeviceDetector::new("../data/51Degrees-LiteV3.2.dat", vec![PropertyName::BrowserVersion]).ok().unwrap();

        let ua = "Mozilla/5.0 (iPhone; CPU iPhone OS 10_3_2 like Mac OS X) AppleWebKit/603.2.4 (KHTML, like Gecko) FxiOS/7.5b3349 Mobile/14F89 Safari/603.2.4".to_string();
        let matched = detector.match_by_user_agent(&ua).unwrap();

        //assert_eq!(matched.get_property(PropertyName::IsSmartPhone).unwrap(), PropertyValue::IsSmartPhone(true));
        //assert_eq!(matched.get_property(PropertyName::DeviceType).unwrap(), PropertyValue::DeviceType(DeviceType::SmartPhone));
        assert_eq!(matched.get_property(PropertyName::BrowserVersion).unwrap(), PropertyValue::BrowserVersion("7.5"));
        //assert_eq!(matched.get_property(PropertyName::PlatformVersion).unwrap(), PropertyValue::PlatformVersion("10.3.2"));
        //assert_eq!(matched.get_property(PropertyName::BrowserName).unwrap(), PropertyValue::BrowserName(BrowserName::Safari));
    }

    #[bench]
    fn pattern_bench(b: &mut Bencher) {
        let detector = PatternDeviceDetector::new("../data/51Degrees-LiteV3.2.dat", vec![PropertyName::BrowserVersion]).ok().unwrap();
        let ua = "Mozilla/5.0 (iPhone; CPU iPhone OS 10_3_2 like Mac OS X) AppleWebKit/603.2.4 (KHTML, like Gecko) FxiOS/7.5b3349 Mobile/14F89 Safari/603.2.4".to_string();

        b.iter(|| {
            let matched = detector.match_by_user_agent(&ua).unwrap();

            assert_eq!(matched.get_property(PropertyName::BrowserVersion).unwrap(), PropertyValue::BrowserVersion("7.5"));
        });
    }
}