var exec = require('child_process').exec;
var os = require('os');
if (os.type() === "Linux")
{
	// Check the GCC version.
	exec('gcc -dumpversion', function callback(error, stdout, stderr) {
		var version = stdout.replace('\n','');
		if (version < 4.7) {
			throw "gcc is version " + version + " but must be 4.7 or greater";
		}
	});
	exec('g++ -dumpversion', function callback(error, stdout, stderr) {
		var version = stdout.replace('\n','');
		if (version < 4.7) {
			throw "g++ is version " + version + " but must be 4.7 or greater.";
		}
	});
}

// Check the node version.
var nodeVersion = process.versions.node;
var nodeVersionMajor = nodeVersion.split(".")[0];
var nodeVersionMinor = nodeVersion.split(".")[1];
console.log(nodeVersionMajor + "." + nodeVersionMinor);
if ((nodeVersionMajor < 6 && nodeVersionMinor < 5) || nodeVersionMajor >= 10) {
	throw "Node version must be >= 6.5 and < 10.";
}
