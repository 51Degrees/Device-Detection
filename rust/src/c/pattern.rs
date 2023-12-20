use crate::c::{Byte, Date};

#[repr(u32)]
#[derive(PartialEq)]
pub enum PatternInitStatus {
    Success = 0,
    InsufficientMemory = 1,
    CorruptData = 2,
    IncorrectVersion = 3,
    FileNotFound = 4,
    NotSet = 5,
    PointerOutOfBounds = 6,
    NullPointer = 7
}

#[repr(u32)]
#[allow(dead_code)]
#[derive(Copy, Clone, PartialEq, Debug)]
pub enum PatternMatchMethod {
    None = 0,
    Exact = 1,
    Numeric = 2,
    Nearest = 3,
    Closest = 4
}

#[repr(C, packed)]
#[allow(non_snake_case)]
#[derive(Debug, Copy, Clone)]
struct Profile {
    componentIndex: Byte,
    profileId: i32,
    valueCount: i32,
    signatureCount: i32,
}

#[repr(C, packed)]
#[allow(non_snake_case)]
#[derive(Debug, Copy, Clone)]
struct ResultSet {
    dataSet: *const DataSet,
    targetUserAgentArray: *mut Byte,
    targetUserAgentArrayLength: u16,
    targetUserAgentHashCode: u64,
    hashCodeSet: Byte,
    method: PatternMatchMethod,
    difference: i32,
    rootNodesEvaluated: i32,
    stringsRead: i32,
    nodesEvaluated: i32,
    signaturesCompared: i32,
    signaturesRead: i32,
    closestSignatures: i32,
    profiles: *mut *const Profile,
    profileCount: i32,
    signature: *mut Byte,
    cache: *const ResultSetCache,
    listPrevious: *mut ResultSet,
    listNext: *mut ResultSet,
    treeParent: *mut ResultSet,
    treeLeft: *mut ResultSet,
    treeRight: *mut ResultSet,
    colour: Byte,
}

#[repr(C)]
#[allow(non_snake_case)]
#[derive(Debug, Copy, Clone)]
pub struct EntityHeader {
    pub startPosition: i32,
    pub length: i32,
    pub count: i32,
}

#[repr(C, packed)]
#[allow(non_snake_case)]
#[derive(Debug, Copy, Clone)]
pub struct Header {
    versionMajor: i32,
    versionMinor: i32,
    versionBuild: i32,
    versionRevision: i32,
    tag: [Byte; 16usize],
    exportTag: [Byte; 16usize],
    copyrightOffset: i32,
    age: i16,
    minUserAgentCount: i32,
    nameOffset: i32,
    formatOffset: i32,
    published: Date,
    nextUpdate: Date,
    deviceCombinations: i32,
    maxUserAgentLength: i16,
    minUserAgentLength: i16,
    lowestCharacter: ::std::os::raw::c_char,
    highestCharacter: ::std::os::raw::c_char,
    maxSignatures: i32,
    signatureProfilesCount: i32,
    signatureNodesCount: i32,
    maxValues: i16,
    csvBufferLength: i32,
    jsonBufferLength: i32,
    xmlBufferLength: i32,
    maxSignaturesClosest: i32,
    maxRank: i32,
    strings: EntityHeader,
    components: EntityHeader,
    maps: EntityHeader,
    properties: EntityHeader,
    values: EntityHeader,
    profiles: EntityHeader,
    signatures: EntityHeader,
    signatureNodeOffsets: EntityHeader,
    nodeRankedSignatureIndexes: EntityHeader,
    rankedSignatureIndexes: EntityHeader,
    nodes: EntityHeader,
    rootNodes: EntityHeader,
    profileOffsets: EntityHeader,
}

#[repr(C, packed)]
#[allow(non_snake_case)]
#[derive(Debug, Copy, Clone)]
struct Component {
    componentId: Byte,
    nameOffset: i32,
    defaultProfileOffset: i32,
    httpHeaderCount: u16,
}


#[repr(C)]
#[allow(non_snake_case)]
#[derive(Debug, Copy, Clone)]
struct Map {
    nameOffset: i32,
}

#[repr(C, packed(2))]
#[allow(non_snake_case)]
#[derive(Debug, Copy, Clone)]
pub struct Value {
    propertyIndex: i16,
    nameOffset: i32,
    descriptionOffset: i32,
    urlOffset: i32,
}

#[repr(C, packed)]
#[allow(non_snake_case)]
#[derive(Debug, Copy, Clone)]
struct Property {
    componentIndex: Byte,
    displayOrder: Byte,
    isMandatory: Byte,
    isList: Byte,
    showValues: Byte,
    isObsolete: Byte,
    show: Byte,
    valueType: Byte,
    defaultValueIndex: i32,
    nameOffset: i32,
    descriptionOffset: i32,
    categoryOffset: i32,
    urlOffset: i32,
    firstValueIndex: i32,
    lastValueIndex: i32,
    mapCount: i32,
    firstMapIndex: i32,
}

#[repr(C, packed(2))]
#[allow(non_snake_case)]
#[derive(Debug, Copy, Clone)]
struct Node {
    position: i16,
    nextCharacterPosition: i16,
    parentOffset: i32,
    characterStringOffset: i32,
    childrenCount: i16,
    numericChildrenCount: i16,
    signatureCount: u16,
}

#[repr(C, packed(4))]
#[allow(non_snake_case)]
#[derive(Debug, Copy, Clone)]
struct HttpHeader {
    headerNameOffset: i32,
    headerName: *const ::std::os::raw::c_char,
}

#[repr(C)]
#[derive(Debug, Copy, Clone)]
struct ProfileIndexStruct {
    count: i32,
    indexes: *mut i32,
}

#[repr(C)]
#[allow(non_snake_case)]
#[derive(Debug, Copy, Clone)]
struct ProfileStructArray {
    initialised: i32,
    profilesStructs: *mut ProfileIndexStruct,
}

#[repr(C)]
#[allow(non_snake_case)]
#[derive(Debug, Copy, Clone)]
struct ProfileOffset {
    profileId: i32,
    offset: i32,
}

#[repr(C, packed)]
#[allow(non_snake_case)]
#[derive(Debug, Copy, Clone)]
pub struct DataSet {
    header: Header,
    memoryToFree: *const ::std::os::raw::c_void,
    sizeOfSignature: i32,
    signatureStartOfStruct: i32,
    requiredProperties: *mut *const Property,
    requiredPropertyCount: i32,
    strings: *const Byte,
    componentsData: *const Byte,
    components: *mut *const Component,
    maps: *const Map,
    properties: *const Property,
    values: *const Value,
    profiles: *const Byte,
    signatures: *const Byte,
    signatureNodeOffsets: *const i32,
    nodeRankedSignatureIndexes: *const i32,
    rankedSignatureIndexes: *const i32,
    nodes: *const Byte,
    rootNodes: *mut *const Node,
    profileOffsets: *const ProfileOffset,
    httpHeadersCount: i32,
    httpHeaders: *mut HttpHeader,
    prefixedUpperHttpHeaders: *mut *const ::std::os::raw::c_char,
    valuePointersArray: *mut ProfileStructArray,
    maxPropertyValueLength: *mut i32,
    fileName: *const ::std::os::raw::c_char,
}

#[repr(C, packed(4))]
#[allow(non_snake_case)]
#[derive(Debug, Copy, Clone)]
struct ResultSetCache {
    dataSet: *const DataSet,
    resultSets: *const ResultSet,
    targetUserAgentArrays: *const Byte,
    profiles: *mut *const Profile,
    listFirst: *mut ResultSet,
    listLast: *mut ResultSet,
    root: ResultSet,
    empty: ResultSet,
    total: i32,
    allocated: i32,
    hits: i32,
    misses: i32,
    switches: i32,
    maxIterations: i32,
}

#[repr(C)]
#[allow(non_snake_case)]
#[derive(Debug, Copy, Clone)]
struct LinkedSignatureListItem {
    next: *mut LinkedSignatureListItem,
    previous: *mut LinkedSignatureListItem,
    rankedSignatureIndex: i32,
    frequency: i32,
}

#[repr(C)]
#[derive(Debug, Copy, Clone)]
struct LinkedSignatureList {
    items: *const LinkedSignatureListItem,
    first: *mut LinkedSignatureListItem,
    last: *mut LinkedSignatureListItem,
    count: i32,
    current: *mut LinkedSignatureListItem,
}

#[repr(C, packed(4))]
#[allow(non_snake_case)]
#[derive(Debug, Copy, Clone)]
struct HttpHeaderWorkset {
    header: *mut HttpHeader,
    headerValue: *const ::std::os::raw::c_char,
    headerValueLength: ::std::os::raw::c_int,
}

#[repr(C, packed(4))]
#[allow(non_snake_case)]
#[derive(Debug, Copy, Clone)]
struct WorksetPool {
    dataSet: *const DataSet,
    cache: *mut ResultSetCache,
    provider: *const Provider,
    size: i32,
    available: i32,
    worksets: *mut *mut Workset,
}

#[repr(C, packed)]
#[allow(non_snake_case)]
#[derive(Debug, Copy, Clone)]
pub struct Workset {
    pub dataSet: *const DataSet,
    targetUserAgentArray: *mut Byte,
    targetUserAgentArrayLength: u16,
    targetUserAgentHashCode: u64,
    hashCodeSet: Byte,
    pub method: PatternMatchMethod,
    difference: i32,
    rootNodesEvaluated: i32,
    stringsRead: i32,
    nodesEvaluated: i32,
    signaturesCompared: i32,
    signaturesRead: i32,
    closestSignatures: i32,
    profiles: *mut *const Profile,
    profileCount: i32,
    signature: *mut Byte,
    cache: *const ResultSetCache,
    pub values: *mut *const Value,
    valuesCount: i32,
    input: *mut ::std::os::raw::c_char,
    targetUserAgent: *mut ::std::os::raw::c_char,
    relevantNodes: *mut ::std::os::raw::c_char,
    closestNodes: *mut ::std::os::raw::c_char,
    signatureAsString: *mut ::std::os::raw::c_char,
    tempheaderlowercase: *mut ::std::os::raw::c_char,
    nodes: *mut *const Node,
    orderedNodes: *mut *const Node,
    nodeCount: i32,
    closestNodeRankedSignatureIndex: i32,
    linkedSignatureList: LinkedSignatureList,
    nextCharacterPositionIndex: i16,
    startWithInitialScore: Byte,
    functionPtrGetScore: ::std::option::Option<
        unsafe extern "C" fn(
            ws: *mut Workset,
            node: *const Node,
        ) -> ::std::os::raw::c_int,
    >,
    functionPtrNextClosestSignature: ::std::option::Option<
        unsafe extern "C" fn(ws: *mut Workset) -> *const Byte,
    >,
    tempProfiles: *mut *const Profile,
    importantHeadersCount: i32,
    importantHeaders: *mut HttpHeaderWorkset,
    associatedPool: *const WorksetPool,
}

#[repr(C, packed(4))]
#[allow(non_snake_case)]
#[derive(Debug, Copy, Clone)]
pub struct Pool {
    pub dataSet: *const DataSet,
    cache: *mut ResultSetCache,
    provider: *const Provider,
    size: i32,
    available: i32,
    worksets: *mut *mut Workset,
}

#[repr(C, packed(4))]
#[allow(non_snake_case)]
#[derive(Debug, Copy, Clone)]
pub struct Provider {
    pub activePool: *mut Pool,
}

extern "C" {
    #[link_name = "fiftyoneDegreesInitProviderWithPropertyString"]
    pub fn init_provider_with_property_String(
        fileName: *const ::std::os::raw::c_char,
        provider: *mut Provider,
        properties: *const ::std::os::raw::c_char,
        poolSize: ::std::os::raw::c_int,
        cacheSize: ::std::os::raw::c_int,
    ) -> PatternInitStatus;

    #[link_name = "fiftyoneDegreesGetRequiredPropertyIndex"]
    pub fn get_required_property_index(
        dataSet: *const DataSet,
        propertyName: *const ::std::os::raw::c_char,
    ) -> i32;

    #[link_name = "fiftyoneDegreesSetValues"]
    pub fn set_values(
        ws: *mut Workset,
        requiredPropertyIndex: i32,
    ) -> i32;

    #[link_name = "fiftyoneDegreesGetValueName"]
    pub fn get_value_name(
        dataSet: *const DataSet,
        value: *const Value,
    ) -> *const ::std::os::raw::c_char;

    #[link_name = "fiftyoneDegreesProviderWorksetGet"]
    pub fn workset_get(
        provider: *mut Provider,
    ) -> *mut Workset;

    #[link_name = "fiftyoneDegreesWorksetRelease"]
    pub fn workset_release(ws: *mut Workset);

    #[link_name = "fiftyoneDegreesMatch"]
    pub fn do_match(
        ws: *mut Workset,
        userAgent: *const ::std::os::raw::c_char,
    );
}