var Parser = require('../index').Parser;
var properties = ["DeviceType"];
var psr = new Parser("51Degrees-Enterprise.dat", ['DeviceType']);

var useragents = [
  "Mozilla/5.0 (X11; Linux i686) AppleWebKit/535.1 (KHTML, like Gecko) Chrome/13.0.782.99 Safari/535.1",
  "Mozilla/5.0 (compatible; MSIE 6.0; Windows NT 5.1)",
  "Mozilla/5.0 (Linux; U; Android 2.2.1; en-ca; LG-P505R Build/FRG83) AppleWebKit/533.1 (KHTML, like Gecko) Version/4.0 Mobile Safari/533.1",
  "Mozilla/5.0 (PLAYSTATION 3; 3.55)"
];

var devices = [
  "desktop",
  "desktop",
  "smartphone",
  "console"
];

var sendRequest = function(x) {

  var number = Math.floor(Math.random() * 2);
  var ua = useragents[number];
  var parse_obj = psr.parse(ua);

  console.log("---------------------------------------------------------------------------------------------\nrequest # " + x + "\nuseragent: " + ua + "\n51Degrees: " + parse_obj.DeviceType  + "\n---------------------------------------------------------------------------------------------\n\n");
  if(parse_obj.DeviceType.toLowerCase() != devices[number]) {
    console.log("fail. expected:", devices[number]);
    process.exit(1);
  }
  setImmediate(function() {
    sendRequest(++x);
  });
};

sendRequest(0);

// console.log(psr.parse(useragents[3]).DeviceType);
// console.log(psr.parse(useragents[2]).DeviceType);
// console.log(psr.parse(useragents[3]).DeviceType);
