
<<<<<<< HEAD
var test = require('tape');
var Parser = require('./index').Parser;
var properties = require('./index').ALL_PROPERTIES;
var userAgent = 'Mozilla/5.0 (Linux; U; Android 4.2.2; en-us; KFTHWI Build/JDQ39) AppleWebKit/537.36 (KHTML, like Gecko) Silk/3.30 like Chrome/34.0.1847.137 Safari/537.36';

var parser = new Parser('51Degrees-Lite.trie', properties);
var ret = parser.parse(userAgent);

console.log(ret);
=======
/* *********************************************************************
* This Source Code Form is copyright of 51Degrees Mobile Experts Limited.
* Copyright 2014 51Degrees Mobile Experts Limited, 5 Charlotte Close,
* Caversham, Reading, Berkshire, United Kingdom RG4 7BY and Yorkshire, 
* Inc. Copyright 2014 Yorkshire, Inc, Guiyang, Guizhou, China
*
* This Source Code Form is the subject of the following patent
* applications, owned by 51Degrees Mobile Experts Limited of 5 Charlotte
* Close, Caversham, Reading, Berkshire, United Kingdom RG4 7BY:
* European Patent Application No. 13192291.6; and
* United States Patent Application Nos. 14/085,223 and 14/085,301.
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0.
*
* If a copy of the MPL was not distributed with this file, You can obtain
* one at http://mozilla.org/MPL/2.0/.
*
* This Source Code Form is "Incompatible With Secondary Licenses", as
* defined by the Mozilla Public License, v. 2.0.
********************************************************************** */

// defines the type Parser from the index 
var Parser = require('./index').Parser;

// an example User-Agent to be used for detection
var userAgent = 'Mozilla/5.0 (Linux; U; Android 4.2.2; en-us; KFTHWI Build/JDQ39) AppleWebKit/537.36 (KHTML, like Gecko) Silk/3.30 like Chrome/34.0.1847.137 Safari/537.36';

// create the device detection parser with the file name passed as an arguement or the default if not provided
var parser = new Parser(process.argv.length > 2 ? process.argv[2] : '../data/51Degrees-LiteV3.2.dat');

// get the results of the device detection as a JSON object
var ret = parser.parse(userAgent);

// output the JSON results to the console
console.log(ret);

// ensure the parser is freed before exit
parser.dispose();

// exit the process
process.exit();
>>>>>>> V32
