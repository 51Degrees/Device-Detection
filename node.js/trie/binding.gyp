{
    "targets": [
        {
            "target_name": "FiftyOneDegreesTrieV3",
            "sources": [
                "../../src/trie/51Degrees.c",
                "../../src/trie/Provider.cpp",
                "../../src/trie/Match.cpp",
                "../../src/threading.c",
                "../../src/trie/51Degrees_node.cpp"
            ],
            "cflags_cc!": ["-fno-exceptions"],
            "cflags": ["<!(node -e \"\
                var v8Version = process.versions.v8;\
                var string = '-DSWIG_V8_VERSION=0x';\
                var arr = v8Version.split('.');\
                for (var i = 0; i < 3; i++) {\
                    if (arr[i].length === 1) {\
                        string += '0';\
                        }\
                    string += arr[i];\
                }\
                console.log(string);\")"]
        }
    ]
}
