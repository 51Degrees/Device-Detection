use crate::c::{Byte, Date};

#[repr(C, packed(2))]
#[allow(non_snake_case)]
#[derive(Debug, Copy, Clone)]
struct DatasetHeader {
    version: u16,
    formatOffset: i32,
    nameOffset: i32,
    tag: [Byte; 16usize],
    published: Date,
    nextUpdate: Date,
    copyrightOffset: i32,
    maxStringLength: u16,
}

#[repr(C)]
#[allow(non_snake_case)]
#[derive(Debug, Copy, Clone)]
pub struct ActiveDataset {
    pub dataSet: *mut Dataset,
    provider: *mut Provider,
    inUse: ::std::os::raw::c_long,
}

#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub struct Provider {
    pub active: *mut ActiveDataset,
}

#[repr(C)]
#[allow(non_snake_case)]
#[derive(Debug, Copy, Clone)]
struct Integers {
    firstElement: *mut i32,
    count: ::std::os::raw::c_uint,
    freeMemory: ::std::os::raw::c_int,
}

#[repr(C)]
#[allow(non_snake_case)]
#[derive(Debug, Copy, Clone)]
struct Property {
    stringOffset: i32,
    componentIndex: i32,
    subIndex: i32,
    headerCount: i32,
    headerFirstIndex: i32,
}

#[repr(C)]
#[allow(non_snake_case)]
#[derive(Debug, Copy, Clone)]
struct Properties {
    firstElement: *mut Property,
    count: ::std::os::raw::c_uint,
    freeMemory: ::std::os::raw::c_int,
}

#[repr(C)]
#[allow(non_snake_case)]
#[derive(Debug, Copy, Clone)]
struct Collection {
    firstByte: *mut Byte,
    count: ::std::os::raw::c_uint,
}

#[repr(C)]
#[allow(non_snake_case)]
#[derive(Debug, Copy, Clone)]
pub struct Dataset {
    header: DatasetHeader,
    memoryToFree: *const ::std::os::raw::c_void,
    fileName: *const ::std::os::raw::c_char,
    devicePropertiesCount: ::std::os::raw::c_int,
    baseDrift: ::std::os::raw::c_int,
    baseDifference: ::std::os::raw::c_int,
    devicesIntegerCount: ::std::os::raw::c_int,
    requiredPropertiesNames: *mut *const ::std::os::raw::c_char,
    prefixedUpperHttpHeaders: *mut *const ::std::os::raw::c_char,
    components: Integers,
    allProperties: Properties,
    httpHeaders: Integers,
    uniqueHttpHeaders: Integers,
    requiredProperties: Integers,
    strings: Collection,
    profiles: Collection,
    devices: Collection,
    nodes: Collection,
}

#[repr(u32)]
#[derive(PartialEq)]
pub enum TrieInitStatus {
    Success = 0,
    InsufficientMemory = 1,
    CorruptData = 2,
    IncorrectVersion = 3,
    FileNotFound = 4,
    NotSet = 5,
    PointerOutOfBounds = 6,
    NullPointer = 7,
    TooManyOpenFiles = 8
}

extern "C" {
    #[link_name = "fiftyoneDegreesInitProviderWithPropertyString"]
    pub fn init_provider_with_property_string(
        fileName: *const ::std::os::raw::c_char,
        provider: *mut Provider,
        properties: *const ::std::os::raw::c_char,
    ) -> TrieInitStatus;

    #[link_name = "fiftyoneDegreesGetValue"]
    pub fn get_value(
        dataSet: *mut Dataset,
        deviceOffset: ::std::os::raw::c_int,
        propertyIndex: ::std::os::raw::c_int,
    ) -> *const ::std::os::raw::c_char;

    #[link_name = "fiftyoneDegreesGetPropertyIndex"]
    pub fn get_property_index(
        dataSet: *mut Dataset,
        value: *const ::std::os::raw::c_char,
    ) -> ::std::os::raw::c_int;

    #[link_name = "fiftyoneDegreesGetDeviceOffset"]
    pub fn get_device_offset(
        dataSet: *mut Dataset,
        userAgent: *const ::std::os::raw::c_char,
    ) -> ::std::os::raw::c_int;
}