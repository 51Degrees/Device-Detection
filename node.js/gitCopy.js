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

var dataFiles = [
    '51Degrees-LiteV3.2.dat',
    '51Degrees-LiteV3.2.trie'
];

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
if (!fs.existsSync('src')) {
    fs.mkdirSync('src');
    fs.mkdirSync('src/pattern');
    fs.mkdirSync('src/trie');
    fs.mkdirSync('src/cityhash');
}

// Make data directory if it does not exist.
if (!fs.existsSync('data')) {
    fs.mkdirSync('data');
}

try {
    // Check a data file exists. This means we are in a git directory.
    stats = fs.lstatSync('../data/51Degrees-LiteV3.2.dat');
    if (stats.isFile()) {
        // Copy source files.
        srcFiles.forEach(function(fileName) {
            copyFile('../src/' + fileName, 'src/' + fileName, errorCheck)
        });
        // Copy data files.
        dataFiles.forEach(function(fileName) {
            copyFile('../data/' + fileName, 'data/' + fileName, errorCheck)
        });
    }
} catch (err) {
    // File does not exist, so don't copy source.
}