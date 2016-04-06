var FOD = require("./51d");

//// Initialise a new Provider.
var provider = new FOD.provider("config.json");

var http = require("http");
http.createServer(function (req, res) {
//    res.write(provider.getDataSetName());
//    res.write(provider.getDataSetFormat());
//    res.write(provider.getDataSetPublishedDate());
//    res.write(provider.getDataSetNextUpdateDate());
//    res.write(provider.getDataSetSignatureCount());
//    res.write(provider.getDataSetDeviceCombinations());
    
    var match = provider.getMatchForHttpHeaders(req.headers);
    
    res.write(match.getValue("BrowserName") + "\n");
    res.write(match.getValue("PlatformName") + "\n");
    res.write(match.getValue("DeviceType") + "\n");

    match.dispose();
    res.end();
}).listen(3000);

//var express = require("express");
//var app = express();
//app.use(function(req, res, next) {
//    req.device = provider.getMatchForHttpHeaders(req.headers).getValue("DeviceType");
//    next();
//})
//app.get("/", function(req, res) {
//    res.send(req.device);
//}).listen(3000);
