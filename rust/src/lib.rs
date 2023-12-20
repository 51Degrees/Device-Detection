#![feature(test)]

extern crate test;

pub mod values;
pub mod common;

mod c;

#[cfg(feature = "trie")]
pub mod trie;

#[cfg(feature = "pattern")]
pub mod pattern;
