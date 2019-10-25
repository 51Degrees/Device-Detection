extern crate libc;

use std::mem;
use std::ffi::{CString, CStr};
use crate::pattern_c::*;
use std::mem::{MaybeUninit, forget};
use std::os::raw::c_char;
use std::pin::Pin;
use crate::common::{PropertyIndexes, PropertyName, DeviceMatch, PROPERTY_INDEXES_EMPTY, DeviceDetector};
use crate::values::PropertyValue;
use std::alloc::{Layout, alloc};
use std::rc::Rc;
use std::borrow::{BorrowMut, Borrow};
use std::ptr::NonNull;
use std::fmt::Display;
use std::cell::{RefCell};

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
    Unexpected,
    InvalidProperty
}

pub struct PatternDeviceDetector{
    provider: RefCell<Box<MaybeUninit<fiftyoneDegreesProvider>>>,
    indexes: PropertyIndexes
}

pub struct TriePropertyAccessor {
    dataset: *mut fiftyoneDegreesDataSet
}

type TrieDeviceIndex = ::std::os::raw::c_int;
type DataSet = *mut fiftyoneDegreesDataSet;

impl PatternDeviceDetector {
    pub fn new(file_location: &str, properties: Vec<PropertyName>) -> Result<PatternDeviceDetector, PatternInitStatus> {
        let provider:MaybeUninit<fiftyoneDegreesProvider> = unsafe { MaybeUninit::uninit()};

        let mut boxed = Box::new(provider);

        let file_name = CString::new(file_location).expect("CString::new failed");

        let mut property_select = String::new();

        for property in &properties {
            property_select.push_str(property.as_str());
            property_select.push_str(",");
        }

        let property_select = CString::new(property_select).expect("CString::new failed");

        let status_code = unsafe {
            fiftyoneDegreesInitProviderWithPropertyString(file_name.into_raw(), boxed.as_mut_ptr(), property_select.into_raw(), 1, 0)
        };

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
            let dataset = unsafe { (*(*boxed.as_ptr()).activePool).dataSet };

            let mut indexes:PropertyIndexes = PROPERTY_INDEXES_EMPTY;

            for property in &properties {
                let c_string = property.as_cstring();

                let index = unsafe { fiftyoneDegreesGetRequiredPropertyIndex(dataset,c_string.as_ptr()) };

                if index == -1 {
                    return Err(PatternInitStatus::InvalidProperty)
                }

                indexes[usize::from(property)] = index;
            }

            Ok(PatternDeviceDetector{
                provider: RefCell::new(boxed),
                indexes
            })
        }else{
            Err(status)
        }
    }
}

pub struct PatternDeviceMatch<'detector> {
    indexes: &'detector PropertyIndexes,
    workset: *mut fiftyoneDegreesWorkset
}

impl<'detector> DeviceMatch for PatternDeviceMatch<'detector>{
    fn get_property(&self, property_type: PropertyName) -> Option<PropertyValue> {
        let str = unsafe {
            let set = fiftyoneDegreesSetValues(self.workset, self.indexes[usize::from(&property_type)]);

            let vals = (*self.workset).values;

            let val = *vals.offset(0);

            let pointer = fiftyoneDegreesGetValueName((*self.workset).dataSet, val);

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

impl<'detector> Drop for PatternDeviceMatch<'detector> {
    fn drop(&mut self) {
        unsafe {
            fiftyoneDegreesWorksetRelease(self.workset);
        }
    }
}


impl<'detector> DeviceDetector<'detector, PatternDeviceMatch<'detector>> for PatternDeviceDetector{
    fn match_by_user_agent(&'detector self, user_agent: &str) -> Option<PatternDeviceMatch<'detector>> {
        let mut pointer = unsafe {
            fiftyoneDegreesProviderWorksetGet(self.provider.borrow_mut().as_mut_ptr())
        };

        let user_agent = CString::new(user_agent).expect("CString::new failed");

        unsafe {
            fiftyoneDegreesMatch(pointer, user_agent.as_ptr());

            if (*pointer).method == e_fiftyoneDegrees_MatchMethod_NONE {
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
    use crate::values::device_type::DeviceType;
    use crate::values::browser_name::BrowserName;

    #[test]
    fn pattern_init() {
        let mut detector = PatternDeviceDetector::new("../data/51Degrees-LiteV3.2.dat", vec![PropertyName::BrowserVersion]).ok().unwrap();

        let mut matched = detector.match_by_user_agent("Mozilla/5.0 (iPhone; CPU iPhone OS 10_3_2 like Mac OS X) AppleWebKit/603.2.4 (KHTML, like Gecko) FxiOS/7.5b3349 Mobile/14F89 Safari/603.2.4").unwrap();

        //assert_eq!(matched.get_property(PropertyName::IsSmartPhone).unwrap(), PropertyValue::IsSmartPhone(true));
        //assert_eq!(matched.get_property(PropertyName::DeviceType).unwrap(), PropertyValue::DeviceType(DeviceType::SmartPhone));
        assert_eq!(matched.get_property(PropertyName::BrowserVersion).unwrap(), PropertyValue::BrowserVersion("7.5"));
        //assert_eq!(matched.get_property(PropertyName::PlatformVersion).unwrap(), PropertyValue::PlatformVersion("10.3.2"));
        //assert_eq!(matched.get_property(PropertyName::BrowserName).unwrap(), PropertyValue::BrowserName(BrowserName::Safari));
    }

    #[bench]
    fn pattern_bench(b: &mut Bencher) {
        let mut detector = PatternDeviceDetector::new("../data/51Degrees-LiteV3.2.dat", vec![PropertyName::BrowserVersion]).ok().unwrap();

        b.iter(|| {
            let mut matched = detector.match_by_user_agent("Mozilla/5.0 (iPhone; CPU iPhone OS 10_3_2 like Mac OS X) AppleWebKit/603.2.4 (KHTML, like Gecko) FxiOS/7.5b3349 Mobile/14F89 Safari/603.2.4").unwrap();

            assert_eq!(matched.get_property(PropertyName::BrowserVersion).unwrap(), PropertyValue::BrowserVersion("7.5"));
        });
    }
}