var zlib = require("zlib");
var http = require("http");

// Share usage object to return.
var shareUsage = {};

// Is a share usage worker already running?
var running = 0;

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
        switch (response.statusCode)    {
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
        log.emit('debug', 'Usage data sent successfully');
    });
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

// Replaces symbols that may be interpreted as part of the xml.
var charEscape = function(str) {
    return str.replace(/\</g,"&lt;")
        .replace(/\>/g,"&gt;")
        .replace(/\"/g, "&quot;");
}

// Records the information as XML data and converts to a string for storage.
var getContent = function(request) {
    // Begin the XML.
    var device = '<Device>';

    // Add the sender information.
    device += '<DateSent>' + new Date().toISOString() + '</DateSent>';
    device += '<Version>' + version + '</Version>';
    device += '<Product>' + product + '</Product>';

    // Get the remote address.
    var remoteAddress = request.connection.remoteAddress;
    // Are you local?
    if (isLocal(remoteAddress.toString()) === true) {
        device += '<ClientIP>' + remoteAddress.toString().replace(/^.*:/, '')
            + '</ClientIP>';
    }

    // Add the local address, removing the leading part.
    var localAddress = request.connection.localAddress.replace(/^.*:/, '');
    device += '<ServerIP>' + localAddress + '</ServerIP>';

    // Add the headers that are useful.
    Object.keys(request.headers).forEach(function (header) {
        if (header === 'user-agent'
            || header === 'host'
            || header.indexOf('profile') !== -1) {
            device += '<Header Name="' + header + '">'
                + charEscape(request.headers[header])
                + '</Header>';
        }
    })

    // End the XML.
    device += '</Device>';
    
    return device;
}

// Module constructor. Sets the error log, product version and product name.
module.exports = function(provider, FOD) {
    if (running !== 1) {
        log = FOD.log;

        if (provider.config.UsageSharingDebug === true) {
            // Provider has been created as part of a test, so send the usage
            // data to localhost after one request to be tested.
            requestOptions.host = 'localhost';
            requestOptions.port = 1234;
            newDeviceQueueLength = 1;
        }
    
        // Get the version of the data set e.g. "3.2".
        version = provider.getDataSetFormat();
    
        // Get the product name e.g. "Node js : Trie"
        product = 'Node js : ' + provider.config.Type;
        log.emit('info', '[' + provider.Id + '] ' +
                 'Usage sharer started');

        // The usage sharer is started.
        running = 1;
    }
    else {
        log.emit('info', '[' + provider.Id + '] ' +
                 'Using pre-existing usage sharer.');
    }
    // Return the share usage object.
    return shareUsage;
};
