use std::str::Utf8Error;
use std::slice::SliceIndex;
use std::ffi::CString;
use crate::values::PropertyValue;

type PropertyLookupFn = fn(i32, i32) -> i32;

pub trait DeviceMatch {
    fn get_property(&self, property_type: PropertyName) -> Option<PropertyValue>;
}

pub trait DeviceDetector<'detector, M:DeviceMatch + 'detector> {
    fn match_by_user_agent(&'detector self, user_agent: &String) -> Option<M>;
}

pub type PropertyIndexes = [i32;10];
pub static PropertyIndexesEmpty:PropertyIndexes = [-1;10];

pub enum PropertyName {
    DeviceType,
    IsSmartPhone,
    IsTablet,
    HardwareName,
    HardwareModel,
    HardwareVendor,
    PlatformName,
    PlatformVersion,
    BrowserName,
    BrowserVersion
}

impl PropertyName {
    pub fn as_str(&self) -> &'static str {
        match self {
            PropertyName::DeviceType => "DeviceType",
            PropertyName::IsSmartPhone => "IsSmartPhone",
            PropertyName::IsTablet => "IsTablet",
            PropertyName::HardwareName => "HardwareName",
            PropertyName::HardwareModel => "HardwareModel",
            PropertyName::HardwareVendor => "HardwareVendor",
            PropertyName::PlatformName => "PlatformName",
            PropertyName::PlatformVersion => "PlatformVersion",
            PropertyName::BrowserName => "BrowserName",
            PropertyName::BrowserVersion => "BrowserVersion"
        }
    }

    pub fn as_cstring(&self) -> CString {
        CString::new(self.as_str()).expect("CString::new failed")
    }
}

impl From<&PropertyName> for usize {
    fn from(property_type: &PropertyName) -> Self {
        match property_type {
            PropertyName::DeviceType => 0,
            PropertyName::IsSmartPhone => 1,
            PropertyName::IsTablet => 2,
            PropertyName::HardwareName => 3,
            PropertyName::HardwareModel => 4,
            PropertyName::HardwareVendor => 5,
            PropertyName::PlatformName => 6,
            PropertyName::PlatformVersion => 7,
            PropertyName::BrowserName => 8,
            PropertyName::BrowserVersion => 9
        }
    }
}
