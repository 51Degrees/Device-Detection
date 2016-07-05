var FiftyOneDegrees = require("../FiftyOneDegreesV3.js");
var provider = new FiftyOneDegrees.provider("config.json");

var assert = require("assert");
describe("User-Agents", function() {
    describe("Good User-Agent", function() {
        it("Should return a valid Match object", function() {
            var i, count = 100000;
            var start = new Date();
            for (i=0;i<count;i++) {
                var match = provider.getMatch("Mozilla/5.0 (Linux; Android 5.1; HTC One M9 Build/LMY47O) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/47.0.2526.83 Mobile Safari/537.36");
                assert.equal("True", match.getValue("IsMobile"));
                match.dispose();
            }
            var end = new Date();
            var timeTaken = end - start;
            var timePerDetection = timeTaken / count;
            console.log(timePerDetection + " ms per detection.");
            var detectionsPerSecond = 1000 / timePerDetection;
            console.log(detectionsPerSecond + " detections per second.");
        })
    })
    describe("Startup", function() {
        it("Should startup in <1.5s", function() {
            var start = new Date();
            var provider = new FiftyOneDegrees.provider("config.json");
            var end = new Date();
            var timeTaken = end - start;
            assert.equal(timeTaken < 150, true);
            console.log(timeTaken + " ms to start up.");
        })
    })
})
