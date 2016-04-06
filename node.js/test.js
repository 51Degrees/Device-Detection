var FOD = require("./51d.js");
var provider = new FOD.provider("config.json");

var assert = require("assert");
describe("User-Agents", function() {
    describe("Good User-Agent", function() {
        it("Should return a valid Match object", function() {
            var i, count = 100000;
            var start = new Date();
            for (i=0;i<count;i++) {
                var match = provider.getMatch("usera");
                assert.equal("False", match.getValue("IsMobile"));
                //match.destroy();
            }
            var end = new Date();
            var timeTaken = end - start;
            var timePerDetection = timeTaken / count;
            console.log(timePerDetection + " ms per detection.");
            var detectionsPerSecond = 1000 / timePerDetection;
            console.log(detectionsPerSecond + " detections per second.");
        })
    })
})