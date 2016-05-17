#!/bin/bash

set -o nounset
set -o errexit

DIR=$(pwd)
BUILDDIR=$DIR/build
NGINX_DIR=nginx
NGINX_VERSION=1.8.1

rm -rf build vendor
if [ ! -d $BUILDDIR ]; then
	mkdir $BUILDDIR > /dev/null 2>&1
	mkdir $BUILDDIR/$NGINX_DIR > /dev/null 2>&1
fi

if [ ! -d "vendor" ]; then
	mkdir vendor > /dev/null 2>&1
fi
if [ ! -d "vendor/nginx" ]; then
	pushd vendor > /dev/null 2>&1
	curl -s -L -O "http://nginx.org/download/nginx-$NGINX_VERSION.tar.gz"
	tar xzf "nginx-$NGINX_VERSION.tar.gz"
	popd > /dev/null 2>&1
	mv "vendor/nginx-$NGINX_VERSION" "vendor/nginx"
	printf "NGINX downloaded to vendor directory\n"
else
	printf "NGINX already downloaded\n"
fi
