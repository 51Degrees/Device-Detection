#!/bin/bash

if [ ! -d 51Degrees_module ] ; then
	mkdir 51Degrees_module
fi
if [ ! -d 51Degrees_module/src ] ; then
	mkdir 51Degrees_module/src
fi
if [ ! -d 51Degrees_module/src/trie ] ; then
	mkdir 51Degrees_module/src/trie
fi
cp ../src/trie_config 51Degrees_module/config
cp ../src/ngx_http_51D_module.c 51Degrees_module/
cp ../../src/trie/51Degrees.c 51Degrees_module/src/trie/
cp ../../src/trie/51Degrees.h 51Degrees_module/src/trie/

