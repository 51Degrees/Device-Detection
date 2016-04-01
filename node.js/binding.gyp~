{
    "targets": [
        {
            "target_name": "FiftyOneDegreesPatternV3",
            "sources": [
                "../src/pattern/51Degrees.c",
                "../src/cityhash/city.c",
		"../src/pattern/Provider.cpp",
		"../src/pattern/Match.cpp",
		"../src/pattern/Profiles.cpp",
		"../src/threading.c",
                "../src/pattern/51Degrees_node.cpp"
            ],
            "cflags_cc!": ["-fno-exceptions"],
            "cflags": ["-DSWIG_V8_VERSION=0x040685"],
            "include_dirs": [
                "<!(node -e \"require('nan')\")"
            ]
        }
    ]
}
