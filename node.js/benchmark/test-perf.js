var Parser = require('../index').Parser;
var trieParser = new Parser('trie');
var patternParser = new Parser('51Degrees-Lite.dat');
var csv = require('csv');
var fs = require("fs");
var path = require('path');
var microtime = require('microtime');

var csvData = fs.readFileSync(path.join(__dirname, './10k.csv'));
csv.parse(csvData,{}, function(err, output) {
    var start = microtime.now();
    var outputLength = output.length; //Cache data for faster speed
    console.log('Parse 10,000 device');
    for (var i = 0; i < outputLength; i++) {
        trieParser.parse(output[i]);
    }
    var trieTotalTime = (microtime.now() - start)/1000000;
    var trieDevicePerSecond = 10000/trieTotalTime;
    console.log('Trie Method: ' + trieTotalTime + ' s || '+ trieDevicePerSecond.toFixed(2) + ' device/s');
    start = microtime.now();
    for (var i = 0; i < outputLength; i++) {
        patternParser.parse(output[i]);
    }
    var patternTotalTime = (microtime.now() - start)/1000000;
    var patternDevicePerSecond = 10000/patternTotalTime;
    console.log('Pattern Method: ' + patternTotalTime + ' s || '+ patternDevicePerSecond.toFixed(2) + ' device/s');
});
