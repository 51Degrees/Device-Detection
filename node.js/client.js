var express = require("express");
var FOD = require("./51d");
//// examples of error logging.
//FOD.log.on("error", function (message) {
//    console.log(message);
//})
//FOD.log.on("updated", function (message) {
//    console.log(message);
//})
//
//// Initialise a new Provider.
var provider = new FOD.provider("config.json");
////process.on("uncaughtException", function(e) {
////    if(e.stack){
////        console.log(e.stack);
////    }
////    console.log(e);
////})
var http = require("http");
http.createServer(function (req, res) {
//    res.write(provider.getDataSetName());
//    res.write(provider.getDataSetFormat());
//    res.write(provider.getDataSetPublishedDate());
//    res.write(provider.getDataSetNextUpdateDate());
//    res.write(provider.getDataSetSignatureCount());
//    res.write(provider.getDataSetDeviceCombinations());
    
    var match = provider.getMatchForHttpHeaders(req.headers);
    
//    res.write(match.getValue("BrowserName"));
//    res.write(match.getValue("PlatformName"));
res.write(match.getDifference().toString());
    //match.destroy();
    res.end();
}).listen(3000);
//
//var app = express();
//app.use(function(req, res, next) {
//    req.device = provider.getMatchForHttpHeaders(req.headers).getValue("DeviceType");
//    next();
//})
//app.get("/", function(req, res) {
//    res.send(req.device);
//}).listen(3000);
