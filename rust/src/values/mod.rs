use crate::values::device_type::DeviceType;
use crate::common::PropertyName;
use crate::values::platform_name::PlatformName;
use crate::values::browser_name::BrowserName;

pub mod property_value;
pub mod device_type;
pub mod platform_name;
pub mod browser_name;

#[derive(PartialEq, Debug)]
pub enum PropertyValue<'detector> {
    DeviceType(DeviceType),
    IsSmartPhone(bool),
    IsTablet(bool),
    HardwareName,
    HardwareModel,
    HardwareVendor,
    PlatformName(PlatformName),
    PlatformVersion(&'detector str),
    BrowserName(BrowserName),
    BrowserVersion(&'detector str)
}

fn value_to_bool(value:&str) -> Option<bool>{
    match value{
        "True" => Some(true),
        "False" => Some(false),
        _ => None
    }
}

impl<'detector> PropertyValue<'detector> {
    pub fn new(property_name: &PropertyName, value: &'detector str) -> Option<PropertyValue<'detector>> {
        match property_name {
            PropertyName::IsSmartPhone => match value_to_bool(value) {
                Some(converted) => Some(PropertyValue::IsSmartPhone(converted)),
                _ => None
            },
            PropertyName::IsTablet => match value_to_bool(value) {
                Some(converted) => Some(PropertyValue::IsTablet(converted)),
                _ => None
            },
            PropertyName::DeviceType => match DeviceType::from_str(value) {
                Some(converted) => Some(PropertyValue::DeviceType(converted)),
                _ => None
            },
            PropertyName::PlatformName => match PlatformName::from_str(value) {
                Some(converted) => Some(PropertyValue::PlatformName(converted)),
                _ => None
            },
            PropertyName::BrowserName => match BrowserName::from_str(value) {
                Some(converted) => Some(PropertyValue::BrowserName(converted)),
                _ => None
            },
            PropertyName::PlatformVersion => Some(PropertyValue::PlatformVersion(value)),
            PropertyName::BrowserVersion =>  Some(PropertyValue::BrowserVersion(value)),
            _ => None
        }
    }
}