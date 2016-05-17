#!/bin/bash

if [ ! -d 51Degrees_module ] ; then
	mkdir 51Degrees_module
fi
if [ ! -d 51Degrees_module/src ] ; then
	mkdir 51Degrees_module/src
fi
if [ ! -d 51Degrees_module/src/pattern ] ; then
	mkdir 51Degrees_module/src/pattern
fi
if [ ! -d 51Degrees_module/src/cityhash ] ; then
	mkdir 51Degrees_module/src/cityhash
fi
cp ../src/pattern_config 51Degrees_module/config
cp ../src/ngx_http_51D_module.c 51Degrees_module/
cp ../../src/pattern/51Degrees.c 51Degrees_module/src/pattern/
cp ../../src/pattern/51Degrees.h 51Degrees_module/src/pattern/
cp -r ../../src/cityhash/ 51Degrees_module/src/
cp ../../src/threading.* 51Degrees_module/src/

