var Parser = require('../index').Parser;
var trieParser = new Parser('trie');
var patternParser = new Parser('51Degrees-Lite.dat');
var csv = require('csv');
var fs = require("fs");
var path = require('path');
var microtime = require('microtime');

if (!gc) throw new Error('run with --expose-gc');

var csvData = fs.readFileSync(path.join(__dirname, './10k.csv'));
csv.parse(csvData,{}, function(err, ualist) {
  printsMemoryUsage();
  ualist.forEach(function(data) {
    var ua = data[0];
    trieParser.parse(ua);
    printsMemoryUsage();
  });

  setTimeout(function() {
    printsMemoryUsage();
    process.exit();
  }, 3*1000);
});

setInterval(function() {
  gc();
}, 100);

function printsMemoryUsage() {
  console.log(process.memoryUsage());
}