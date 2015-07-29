{
    "targets": [
        {
            "target_name": "pattern",
            "sources": [
                "../src/pattern/51Degrees.c",
                "../src/cityhash/city.c",
                "src/pattern/api.cc"
            ],
            "cflags": [
                "-Wno-trigraphs"
            ],
            "defines": [
                "HAVE_SNPRINTF"
            ],
            "include_dirs": [
                "<!(node -e \"require('nan')\")"
            ],
        },
        {
            "target_name": "trie",
            "sources": [
                "../src/trie/51Degrees.c",
                "src/trie/api.cc"
            ],
            "cflags": [
                "-Wno-trigraphs"
            ],
            "defines": [
                "HAVE_SNPRINTF"
            ],
            "include_dirs": [
                "<!(node -e \"require('nan')\")"
            ]
        }
    ]
}
