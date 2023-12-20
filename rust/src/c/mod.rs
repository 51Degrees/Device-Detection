type Byte = ::std::os::raw::c_uchar;

#[repr(C, packed)]
#[derive(Debug, Copy, Clone)]
struct Date {
    year: i16,
    month:Byte,
    day: Byte,
}

#[cfg(feature = "pattern")]
pub mod pattern;

#[cfg(feature = "trie")]
pub mod trie;
