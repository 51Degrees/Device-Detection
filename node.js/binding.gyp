{
  "targets": [
    {
      "target_name": "pattern",
      "sources": [
        "src/snprintf/snprintf.c",
        "src/pattern/51Degrees.c",
        "src/pattern/api.cc",
      ],
      "cflags": [
        "-Wno-trigraphs"
      ],
      "defines": [
        "HAVE_SNPRINTF"
      ],
      "include_dirs" : [
        "<!(node -e \"require('nan')\")"
      ],
    },
    {
      "target_name": "trie",
      "sources": [
        "src/snprintf/snprintf.c",
        "src/trie/51Degrees.c",
        "src/trie/api.cc",
      ],
      "cflags": [
        "-Wno-trigraphs"
      ],
      "defines": [
        "HAVE_SNPRINTF"
      ],
      "include_dirs" : [
        "<!(node -e \"require('nan')\")"
      ],
    },
  ]
}
