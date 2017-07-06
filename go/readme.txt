Alpha development with examples.

Dependencies:
-------------
gcc 5.x
g++ 5.x
Swig 3.0.12
Make
golang 1.8

Examples:
---------
FindProfiles.go
GettingStarted.go
MatchForDeviceId.go
MatchMetrics.go
OfflineProcessing.go
server.go
StronglyTyped.go

run make to copy sources from ../src/pattern directory and build swig wrapper

run with:
	go run example.go

or build and run with:
	go build example.go
	./example

Otional:
--------
Rebuilding SWIG wrapper:
------------------------
src directory has pattern sources only, and the swig generated wrappers for them.

swig wrappers are generated in the src/pattern directory with
	swig -go -cgo -intgosize 64 -c++ 51Degrees.i
