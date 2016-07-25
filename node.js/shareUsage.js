var xmlBuilder = require("xmlbuilder");
var zlib = require("zlib");
var http = require("http");

// Share usage object to return.
var shareUsage = {};

// Queue to store new device information.
var queue = [],
// Maximum length of the queue.
    newDeviceQueueLength = 50;

// Array of local addresses to check client address against.
var local = ['127.0.0.1', '0:0:0:0:0:0:0:1'];

// 51Degrees log to send messages to.
var log;

// Product and version used when sharing usage.
var version,
    product;

// Boolean value used to stop sharing usage if there is a connection
// problem.
var stop = false;

// Url to send device information to.
var requestOptions = {
    host: 'devices.51degrees.mobi',
    path: '/new.ashx',
    method: 'POST',
    port: '80',
    headers: {
        'Content-Type': 'text/xml; charset=utf-8',
        'Content-Encoding': 'gzip'
    }
};

// Indicates if the device is local.
var isLocal = function(address) {
    var isLocal = false;
    local.forEach(function(localAddress) {
        if (address.indexOf(localAddress) !== -1) {
            isLocal = true;
        }
    })
    return isLocal;
}

// Sends all the data in the queue.
var sendData = function(outputStream) {
    log.emit('debug', 'Sending usage data to ' + requestOptions.host);
    var request = http.request(requestOptions, function(response) {
        switch (response.statusCode) {
            case 200:// OK
                // Ok response, do nothing
                break;
            case 408:// Request Timeout
                // Could be temporary, do nothing.
                log.emit('debug', "Response code is 408 : " + response.statusMessage);
                break;
            default:
                // Turn off functionality.
                log.emit('error', 'Stopping usage sharing as remote ' +
                        'name ' + requestOptions.host + ' returned status ' +
                        'description ' + response.statusMessage);
                stop = true;
                break;
        }
    })
    .on('error', function(err) {
        if (err['code'] === 'ENOTFOUND') {
            // The address was not found, stop sharing.
            stop = true;
            log.emit('error', 'Stopping usage sharing as remote name ' + 
                     requestOptions+ ' generated ENOTFOUND exception.')
        } else {
            // Some other error occured, stop sharing.
            stop = true;
            log.emit('error', 'Stopping usage shareing after ' + err['code'] +
                     ' exception.');
        }
    })
    // Set the timeout to 1 second.
    .setTimeout(1000);

    // Begin the devices XML string
    var xmlString = '<?xml version="1.0" encoding="UTF-8"?>';
    xmlString += '<Devices>';
    
    // Add each device to the XML and remove from the queue.
    while (queue.length > 0) {
        // Using pop instead of shift lists devices in reverse order
        // but is quicker.
        xmlString += queue.pop();
    }

    // End the devices XML string.
    xmlString += '</Devices>';

    // Compress the XML string and send it.
    zlib.gzip(xmlString, function(err, result) {
        request.write(result);
        request.end();
    });
    
    log.emit('debug', 'Usage data sent successfully');
}

// Adds the request to the queue to be processed.
shareUsage.recordNewDevice = function(request) {
    if (stop === false) {

        // Usage sharing has not been stopped, so get the device information.
        device = getContent(request);

        // Add the device information to the queue.
        queue.push(device);

        if (queue.length === newDeviceQueueLength) {
            // The queue has reached is maximum length, so send the data.
            sendData(queue);
        }
    }
}

// Records the information as XML data and converts to a string for storage.
var getContent = function(request) {

    // Begin the XML.
    var device = xmlBuilder.create('Device');

    // Add the sender information.
    device.ele('DateSent', new Date().toISOString())
    device.ele('Version', version)
    device.ele('Product', product)

    // Get the remote address.
    var remoteAddress = request.connection.remoteAddress;
    // Are you local?
    if (isLocal(remoteAddress.toString()) === true) {
        // Strip the leading part of the ip when setting.
        device.ele('ClientIP', remoteAddress.replace(/^.*:/, '').toString());
    }

    // Add the local address, removing the leading part.
    var localAddress = request.connection.localAddress.replace(/^.*:/, '');
    device.ele('ServerIP', localAddress);

    // Add the headers that are useful.
    Object.keys(request.headers).forEach(function (header) {
        if (header === 'user-agent'
            || header === 'host'
            || header.indexOf('profile') !== -1) {
            device.ele('Header', {'Name': header}, request.headers[header]);        
        }
    })

    // End the XML.
    device.end();
    
    // Convert the XML to a string.
    return device.toString();    
}

// Module constructor. Sets the error log, product version and product name.
module.exports = function(provider, FOD) {
    log = FOD.log;
    version = provider.getDataSetFormat();
    product = 'Node js : ' + provider.config.Type;
    log.emit('info', 'Usage sharer started')
    return shareUsage;
};