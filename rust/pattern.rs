extern crate libc;

use std::mem;
use std::ffi::{CString, CStr};
use crate::pattern_c::*;
use std::mem::{MaybeUninit, forget};
use std::ptr::slice_from_raw_parts;
use std::intrinsics::copy_nonoverlapping;
use std::os::raw::c_char;

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
    provider: fiftyoneDegreesProvider
}

impl DeviceDetector {
    fn new(file_location: &str, properties: Vec<DeviceProperties>) -> Result<DeviceDetector, PatternInitStatus> {
        let mut provider = unsafe { MaybeUninit::uninit()};

        let file_name = CString::new(file_location).expect("CString::new failed");
        let file_name_ptr = file_name.into_raw();

        let properties = CString::new("IsMobile,DeviceType,IsTablet").expect("CString::new failed");
        let properties_ptr = properties.into_raw();

        let status_code = unsafe {
            fiftyoneDegreesInitProviderWithPropertyString(file_name_ptr, provider.as_mut_ptr() , properties_ptr,1, 0)
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
            let provider = unsafe { provider.assume_init() };

            // we will let the c-library handling deallocation
            std::mem::forget(provider);

            Ok(DeviceDetector{
                provider
            })
        }else{
            Err(status)
        }
    }

    fn detect(&mut self, user_agent:&str){
        let agent = CString::new(user_agent).expect("CString::new failed");
        let agent_raw = agent.into_raw();

        unsafe {
            let ws = fiftyoneDegreesProviderWorksetGet(&mut self.provider);

            //println!("available {}", (*self.provider.activePool).available);

           //println!("method {}", (*ws).method);

           //fiftyoneDegreesMatch(ws, agent_raw);


            //println!("method {} ", (*ws).method);

            //let dataset = (*ws).dataSet;

            /*
            let is_mobile = CString::new("IsMobile").expect("CString::new failed");
            let is_mobile_ptr = is_mobile.as_ptr();
            let is_mobile_index = fiftyoneDegreesGetRequiredPropertyIndex(dataset, is_mobile_ptr);

            if is_mobile_index >= 0 {
                let set = fiftyoneDegreesSetValues(ws, is_mobile_index);

                let vals = (*ws).values;

                let val = *vals.offset(0);

                let ptr = fiftyoneDegreesGetValueName(dataset, val);

                let c_str = CStr::from_ptr(ptr);

                println!("unwrapped {}",c_str.to_str().unwrap());
            }
            */

            fiftyoneDegreesWorksetRelease(ws);

            // we will let the c-library handling deallocation
            //forget(ws);

            //println!("available {}", (*self.provider.activePool).available);
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use test::Bencher;

    #[test]
    fn pattern_init() {
        let mut result = DeviceDetector::new("../data/51Degrees-LiteV3.2.dat", vec![DeviceProperties::IsMobile,DeviceProperties::DeviceType]).ok().unwrap();

        result.detect("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_14_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/77.0.3865.120 Safari/537.36");
        result.detect("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_14_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/77.0.3865.120 Safari/537.36");
    }

    #[bench]
    fn bench_pow(b: &mut Bencher) {
        let mut result = DeviceDetector::new("../data/51Degrees-LiteV3.2.dat", vec![DeviceProperties::IsMobile,DeviceProperties::DeviceType]).ok().unwrap();

        b.iter(|| {
            result.detect("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_14_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/77.0.3865.120 Safari/537.36");
        });
    }
}