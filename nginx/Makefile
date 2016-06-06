ifeq (build,$(firstword $(MAKECMDGOALS)))
	API := $(word 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
	ARGNEEDED := 1
endif
ifeq (install,$(firstword $(MAKECMDGOALS)))
	API := $(word 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
	ARGNEEDED := 1
endif
$(eval $(API):;@:)

ifdef ARGNEEDED
ifeq ($(API),pattern)
	ARGS := "-DFIFTYONEDEGREES_PATTERN -DFIFTYONEDEGREES_NO_THREADING -Wno-ignored-qualifiers -Wno-unused-result"
else ifeq ($(API),trie)
	ARGS := "-DFIFTYONEDEGREES_TRIE"
else
$(error $(API) is not a valid argument, must be pattern or trie)
endif
endif

.PHONY pattern:

.PHONY trie:

clean:
	if [ -d "51Degrees_module/src" ]; then \
		rm -rf 51Degrees_module/src; \
	fi
	if [ -d "build" ]; then \
		rm -rf build; \
	fi
	if [ -f "nginx" ]; then \
		rm -f nginx; \
	fi
	if [ -f "vendor/nginx/Makefile" ]; then \
		cd $(CURDIR)/vendor/nginx && make clean; \
	fi

build: clean
	mkdir -p 51Degrees_module/src/$(API)
	if [ -d "51Degrees_module/src/pattern" ]; then \
		mkdir -p 51Degrees_module/src/cityhash && \
		cp -r ../src/cityhash/ 51Degrees_module/src/ && \
		cp ../src/threading.* 51Degrees_module/src/ ; \
	fi
	exit
	cp module_conf/$(API)_config 51Degrees_module/config
	cp ../src/$(API)/51Degrees.c 51Degrees_module/src/$(API)/
	cp ../src/$(API)/51Degrees.h 51Degrees_module/src/$(API)/
	

get-source:
	if [ -d "vendor" ]; then rm -r vendor; fi
	mkdir -p vendor/nginx
	curl -L -O "http://nginx.org/download/nginx-1.10.0.tar.gz"
	tar xzf "nginx-1.10.0.tar.gz"
	mv nginx-1.10.0.tar.gz vendor/
	mv -f -u nginx-1.10.0/* vendor/nginx
	rmdir nginx-1.10.0

install: build
	if [ ! -d "vendor/nginx" ]; then $(MAKE) get-source; fi
	cd $(CURDIR)/vendor/nginx && \
	./configure \
	--prefix=$(CURDIR)/build \
	--with-ld-opt="-lm" \
	--add-module=$(CURDIR)/51Degrees_module \
	--with-cc-opt=$(ARGS) \
	--sbin-path=$(CURDIR) \
	--conf-path="nginx.conf"
	cd $(CURDIR)/vendor/nginx && make install
	cp example/$(API).conf build/nginx.conf

test:
	./test.sh
