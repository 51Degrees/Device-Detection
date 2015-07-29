
var Parser = require('./index').Parser;
var properties = require('./index').ALL_PROPERTIES;
var userAgent = 'Mozilla/5.0 (Linux; U; Android 4.2.2; en-us; KFTHWI Build/JDQ39) AppleWebKit/537.36 (KHTML, like Gecko) Silk/3.30 like Chrome/34.0.1847.137 Safari/537.36';

var parser = new Parser('../data/51Degrees-LiteV3.2.trie', properties);
var ret = parser.parse(userAgent);

console.log(ret);