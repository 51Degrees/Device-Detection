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
console.log(nodeVersionMajor);
if (nodeVersionMajor >= 7) {
	throw "Node version must be < v7. For a beta version that supports 7 and up, use 'npm install fiftyonedegreescore@dev'";
}
