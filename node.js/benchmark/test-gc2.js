var path = require('path');
var fs = require('fs');
var Parser = require('../index').Parser;
var properties = [
  "DeviceType",
  "HardwareFamily",
  "HardwareModel",
  "PlatformName",
  "BrowserVendor",
  "BrowserName",
  "BrowserVersion",
  "IsCrawler",
  "PriceBand",
  "HasTouchScreen"
];

var psr = new Parser('51Degrees-Lite.dat', properties);
var ua_array = fs.readFileSync(path.join(__dirname, './ua.txt')).toString().split("\n");

var doIt = function(array, cb) {
  if (array.length == 0) {
    cb();
  } else {
    var arr = array.shift();
    setImmediate(function() {
      psr.parse(arr);
      doIt(array, cb);
    });
  }
}


var looper = function(){
  console.log("Ok we're done with one round...")
  doIt(ua_array.slice(), looper);
};

doIt(ua_array.slice(), looper);

setInterval(gc, 500);