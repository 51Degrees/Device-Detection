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

var test = require('tape');
var crypto = require('crypto');
var fs = require('fs');
var zlib = require('zlib');
var path = require('path');
var Parser = require('./index').Parser;
var configmock = require('httpmocker').config;

var patternFileName = process.argv.length > 2 ? process.argv[2] : '../data/51Degrees-LiteV3.2.dat';
var trieFileName = process.argv.length > 3 ? process.argv[3] : '../data/51Degrees-LiteV3.2.trie';
var userAgent = 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_9_4) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/37.0.2062.94 Safari/537.36';
var ua_src = fs.readFileSync(path.join(__dirname, '../data/20000 User Agents.csv'));
var ua_array = (ua_src + '').split('\n');

// Change to Premium or Enterprise licence key string.
var licenceKey = 'PROVIDE YOUR LICENCE KEY';

test('constructor arguments valid', function (t) {
    try {
        new Parser('51Degrees-Lite.1');
    } catch (e) {
        t.equal(e.code, 'DB_NOT_FOUND');
        t.end();
    }
});

test('pattern', function (t) {
    var parser = new Parser(patternFileName);
    var ret = parser.parse(userAgent);
    parser.availableProperties().forEach(function (property) {
        t.ok(typeof ret[property] !== undefined, property + '> ok');
    });
    t.equal(ret.method, 'pattern');
    t.end();
});

test('trie', function (t) {
    var parser = new Parser(trieFileName);
    var ret = parser.parse(userAgent);
    parser.availableProperties().forEach(function (property) {
        t.ok(typeof ret[property] !== undefined, property + '> ok');
    });
    t.equal(ret.method, 'trie');
    t.end();
});

test('pattern overflow', function (t) {
    var ua = new Buffer(1000);
    for (var i = 0; i < 1000; i++) {
        ua[i] = 50;
    }
    var parser = new Parser(patternFileName);
    var throwed = false;
    try { parser.parse(ua); }
    catch (e) {
        throwed = true;
    }
    t.equal(throwed, true);
    t.end();
});

test('empty userAgent', function (t) {
    var r;
    var psr1 = new Parser(patternFileName);
    r = psr1.parse('');
    t.equal(r.method, 'pattern');

    var psr2 = new Parser(trieFileName);
    r = psr2.parse('');
    t.equal(r.method, 'trie');
    t.end();
});

test('random userAgent', function (t) {
    var r;
    var ua = crypto.pseudoRandomBytes(10);
    var psr1 = new Parser(patternFileName);
    r = psr1.parse(ua);
    t.equal(r.method, 'pattern');

    var psr2 = new Parser(trieFileName);
    r = psr2.parse(ua);
    t.equal(r.method, 'trie');
    t.end();
});

test('update with 200', function (t) {
    var update = require('./update');
    // Check the licence key isn't the default value otherwise the test
    // will fail due to an invalid licence key.
    if (licenceKey != 'PROVIDE YOUR LICENCE KEY') {
        zlib.gzip(new Buffer('abcdefg', 'utf8'), function (err, data) {
            configmock({
                'https://51degrees.com/Products/': {
                    statusCode: 200,
                    headers: {
                        'content-type': 'text/plain',
                        'content-md5': '519fdca9541cfdfbd905e3a92ec2f8ed'
                    },
                    body: data
                }
            });
            update(licenceKey, 'test.update', function (updated) {
                t.equal(updated, true);
                var d = fs.readFileSync('test.update').toString();
                t.equal(d, 'abcdefg');
                t.end();
                fs.unlinkSync('test.update');
            });
        });
    }
});

test('update with 403', function (t) {
    var update = require('./update');
    configmock({
        'https://51degrees.com/Products/': {
            statusCode: 403
        }
    });
    update(licenceKey, 'test.update', function (updated) {
        t.equal(updated, false);
        t.end();
    });
});

test('auto update', function (t) {
    var sinon = require('sinon');
    var clock = sinon.useFakeTimers();
    var orgSrc = fs.readFileSync(patternFileName);

    zlib.gzip(new Buffer('abcdefg', 'utf8'), function (err, data) {
        configmock({
            'https://51degrees.com/Products/': {
                statusCode: 200,
                headers: {
                    'content-type': 'text/plain',
                    'content-md5': '519fdca9541cfdfbd905e3a92ec2f8ed'
                },
                body: data
            }
        });

        psr1 = new Parser(patternFileName, properties, {
            autoUpdate: true,
            key: licenceKey,
            onupdated: onupdated
        });

        clock.tick(30 * 60 * 1000);
    });

    function onupdated(updated) {
        t.equal(fs.readFileSync(patternFileName).length, 7);
        end();
    }

    function end() {
        fs.writeFileSync(patternFileName, orgSrc);
        clock.restore();
        t.end();
    }
});

test('memory leak at pattern', function (t) {
    var cp1 = process.memoryUsage().rss / 1024 / 1024;
    var psr1 = new Parser(patternFileName);
    ua_array.forEach(function (ua) {
        patternFileName
        psr1.parse(ua);
    });
    var cp2 = process.memoryUsage().rss / 1024 / 1024;
    t.ok(cp2 - cp1 <= 100, 'no memory leak at pattern');
    t.end();
});

test('memory leak at trie', function (t) {
    var cp1 = process.memoryUsage().rss / 1024 / 1024;
    var psr1 = new Parser(trieFileName);
    ua_array.forEach(function (ua) {
        psr1.parse(ua);
    });
    var cp2 = process.memoryUsage().rss / 1024 / 1024;
    t.ok(cp2 - cp1 <= 100, 'no memory leak at trie');
    t.end();
});