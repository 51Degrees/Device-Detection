fn main() {
    if cfg!(feature = "trie") {
        cc::Build::new()
            .define("FIFTYONEDEGREES_NO_THREADING", None)
            .file("../src/threading.c")
            .file("../src/trie/51Degrees.c")
            .compile("trie");
    }

    if cfg!(feature = "pattern") {
        cc::Build::new()
            .define("FIFTYONEDEGREES_NO_THREADING", None)
            .file("../src/threading.c")
            .file("../src/pattern/51Degrees.c")
            .compile("pattern");
    }
}