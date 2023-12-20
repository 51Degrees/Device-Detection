fn main() {
    if cfg!(feature = "trie") {
        cc::Build::new()
            .define("FIFTYONEDEGREES_NO_THREADING", None)
            .warnings(false)
            .file("src/cityhash/city.c")
            .file("src/trie/51Degrees.c")
            .compile("trie");
    }

    if cfg!(feature = "pattern") {
        cc::Build::new()
            .define("FIFTYONEDEGREES_NO_THREADING", None)
            .warnings(false)
            .file("src/cityhash/city.c")
            .file("src/pattern/51Degrees.c")
            .compile("pattern");
    }
}