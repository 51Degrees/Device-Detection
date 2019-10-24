#![feature(test)]

extern crate libc;
extern crate test;

pub mod values;
pub mod common;

#[cfg(feature = "trie")]
mod trie_c;

#[cfg(feature = "trie")]
pub mod trie;

#[cfg(feature = "pattern")]
mod pattern_c;

#[cfg(feature = "pattern")]
pub mod pattern;
