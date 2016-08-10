var fs = require('fs');

var srcFiles = [
    // Threading.
    'threading.c',
    'threading.h',
    // Pattern.
    'pattern/51Degrees_node.cpp',
    'pattern/51Degrees.c',
    'pattern/51Degrees.h',
    'pattern/Provider.cpp',
    'pattern/Provider.hpp',
    'pattern/Match.cpp',
    'pattern/Match.hpp',
    'pattern/Profiles.cpp',
    'pattern/Profiles.hpp',
    // Trie.
    'trie/51Degrees_node.cpp',
    'trie/51Degrees.c',
    'trie/51Degrees.h',
    'trie/Provider.cpp',
    'trie/Provider.hpp',
    'trie/Match.cpp',
    'trie/Match.hpp',
    // CityHash.
    'cityhash/city.c',
    'cityhash/city.h',
    'cityhash/LICENSE'
];

var patternDataFile = '51Degrees-LiteV3.2.dat';
var trieDataFile = '51Degrees-LiteV3.2.trie';

var errorCheck = function(err) {
    if (err) {
        // Just print the error, don't break.
        console.log(err);
    }
}

var copyFile = function(source, target, callback) {
    var calledback = false;

    // Copy the input file.
    var rd = fs.createReadStream(source);
    rd.on("error", function(err) {
        done(err);
    });
    
    // To the output file.
    var wr = fs.createWriteStream(target);
    wr.on("error", function(err) {
        done(err);
    });
    wr.on("close", function(ex) {
        done();
    });
    rd.pipe(wr);

    // Finish the function.
    function done(err) {
        if (!calledback) {
            callback(err);
            calledback = true;
        }
    }
}

// Make source directories if they do not exist.
if (!fs.existsSync('core/src')) {
    fs.mkdirSync('core/src');
    fs.mkdirSync('core/src/pattern');
    fs.mkdirSync('core/src/trie');
    fs.mkdirSync('core/src/cityhash');
}

// Copy source files.
srcFiles.forEach(function(fileName) {
    copyFile('../src/' + fileName, 'core/src/' + fileName, errorCheck)
});

// Copy data files.
copyFile('../data/' + patternDataFile, 'litePattern/' + patternDataFile, errorCheck);
copyFile('../data/' + trieDataFile, 'liteTrie/' + trieDataFile, errorCheck);
