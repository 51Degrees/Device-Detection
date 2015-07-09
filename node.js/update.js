

var fs = require('fs');
var zlib = require('zlib');
var path = require('path');
var util = require('util');
var crypto = require('crypto');
var request = require('https').get;
var format = require('util').format;
var Transform = require('stream').Transform;

var host = 'https://51degrees.com';
var path = '/Products/Downloads/Premium/LicenseKeys/%s/Download/True/Type/BinaryV3';

function update(key, filename, callback) {
  if (!key || typeof key !== 'string')
    throw new Error('invalid LicenseKey');
  if (!filename || typeof filename !== 'string')
    throw new Error('invalid filename');

  // load mtime
  fs.stat(filename, function getstat(err, stat) {
    var option = {
      host: '51degrees.com',
      method: 'GET',
      path: format(path, key),
      // set Last-Modified via before stat.mtime
      headers: {'Last-Modified': stat && stat.mtime}
    };

    request(option, function onresponse(response) {
      if (response.statusCode === 200 && response.headers['content-md5']) {
        // backup the original file to xxx.backup
        backup(filename, function () {
          var ws = fs.createWriteStream(filename);
          var hs = new HashStream('md5');
          hs.on('finish', function() {
            // check md5 match
            var fmd5 = hs.digest('hex');
            var isValid = (response.headers['content-md5'] === fmd5);
            ws.on('finish', function() {
              if (isValid) {
                cleanBackup(filename);
                callback(true);
              } else {
                resume(filename, callback);
              }
            });
          });
          response.pipe(hs).pipe(zlib.createGunzip()).pipe(ws);
        });
      } else {
        callback(false);
      }
    });
  });
}

function backup(filename, callback) {
  var rs = fs.createReadStream(filename);
  var ws = fs.createWriteStream(filename + '.backup');
  rs.on('error', callback);
  ws.on('finish', callback);
  rs.pipe(ws);
}

function resume(filename, callback) {
  fs.unlink(filename, function() {
    var rs = fs.createReadStream(filename + '.backup');
    var ws = fs.createWriteStream(filename);
    rs.on('error', _callback);
    ws.on('finish', _callback);
    rs.pipe(ws);
  });

  function _callback() {
    cleanBackup(filename);
    // resume means the progress failed
    callback(false);
  }
}

function cleanBackup(filename) {
  fs.unlink(filename + '.backup');
}

function HashStream(algorithm, options) {
  if (!(this instanceof HashStream))
    return new HashStream(algorithm, options);
  this.hash = crypto.createHash(algorithm);
  Transform.call(this, options);
}
util.inherits(HashStream, Transform);

HashStream.prototype._transform = function(chunk, encoding, callback) {
  this.hash.update(chunk, encoding);
  this.push(chunk);
  callback();
};

HashStream.prototype.digest = function(encoding) {
  return this.hash.digest(encoding);
};

module.exports = update;
