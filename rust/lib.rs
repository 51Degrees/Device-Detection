#![feature(test)]
#![feature(slice_from_raw_parts)]

extern crate libc;
extern crate test;

#[cfg(feature = "trie")]
mod trie_c;

#[cfg(feature = "trie")]
pub mod trie;

#[cfg(feature = "pattern")]
mod pattern_c;

#[cfg(feature = "pattern")]
pub mod pattern;