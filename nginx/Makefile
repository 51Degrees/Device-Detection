VERSION := 1.13.4
TESTS := tests/51degrees.t


ifeq (build,$(firstword $(MAKECMDGOALS)))
	ARGNEEDED := 1
endif
ifeq (install,$(firstword $(MAKECMDGOALS)))
	ARGNEEDED := 1
endif
ifeq (configure,$(firstword $(MAKECMDGOALS)))
	ARGNEEDED := 1
endif
ifeq (module,$(firstword $(MAKECMDGOALS)))
	ARGNEEDED := 1
endif

$(eval $(API):;@:)

ifdef ARGNEEDED
	API := $(word 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
ifeq ($(API),pattern)
	ARGS := "-DFIFTYONEDEGREES_PATTERN -DFIFTYONEDEGREES_NO_THREADING -Wno-ignored-qualifiers -Wno-unused-result -Wno-unused-but-set-variable -Wno-unused-function"
else ifeq ($(API),trie)
	ARGS := "-DFIFTYONEDEGREES_TRIE -DFIFTYONEDEGREES_NO_THREADING"
else
$(error $(API) is not a valid argument, must be pattern or trie)
endif
endif

ifeq (test,$(firstword $(MAKECMDGOALS)))
FULLPATH := $(shell pwd)
ifneq (,$(findstring TRIE,"$(shell ./nginx -V 2>&1)"))
	DATAFILE := "51Degrees-LiteV3.4.trie"
endif
ifneq (,$(findstring PATTERN,"$(shell ./nginx -V 2>&1)"))
	DATAFILE := "51Degrees-LiteV3.2.dat"
endif
endif

ifndef STATIC_BUILD
	BUILD_MODE := dynamic
	MODULE_ARG := --add-dynamic-module
else
	BUILD_MODE := static
	MODULE_ARG := --add-module
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
	if [ -f "vendor/nginx-$(VERSION)/Makefile" ]; then \
		cd $(CURDIR)/vendor/nginx-$(VERSION) && make clean; \
	fi
	if [ -d "tests/nginx-tests" ]; then \
		rm -r tests/nginx-tests; \
	fi;

build: clean
	mkdir -p 51Degrees_module/src/$(API)
	if [ -d "51Degrees_module/src/pattern" ]; then \
		mkdir -p 51Degrees_module/src/cityhash && \
		cp ../src/cityhash/* 51Degrees_module/src/cityhash/ ; \
	fi
	exit
	cp module_conf/$(API)_config 51Degrees_module/config
	cp ../src/$(API)/51Degrees.c 51Degrees_module/src/$(API)/
	cp ../src/$(API)/51Degrees.h 51Degrees_module/src/$(API)/


get-source:
	if [ ! -d "vendor" ]; then mkdir vendor; fi
	cd vendor && curl -L -O "http://nginx.org/download/nginx-$(VERSION).tar.gz"
	cd vendor && tar xzf "nginx-$(VERSION).tar.gz"

configure: build
	if [ ! -d "vendor/nginx-$(VERSION)" ]; then $(MAKE) get-source; fi
	cd $(CURDIR)/vendor/nginx-$(VERSION) && \
	./configure \
	--prefix=$(CURDIR)/build \
	--with-ld-opt="-lm" \
	$(MODULE_ARG)=$(CURDIR)/51Degrees_module \
	--with-compat \
	--with-cc-opt=$(ARGS) \
	--with-debug \
	--sbin-path=$(CURDIR) \
	--conf-path="nginx.conf"

install: configure
	cd $(CURDIR)/vendor/nginx-$(VERSION) && make install
	cp example/$(API).conf build/nginx.conf

module: configure
	cd $(CURDIR)/vendor/nginx-$(VERSION) && make modules
	if [ ! -d "build/modules" ]; then mkdir -p build/modules; fi
	cp $(CURDIR)/vendor/nginx-$(VERSION)/objs/*.so build/modules

all-versions:
	$(foreach api, \
	pattern trie, \
		$(foreach version, \
		1.11.10 1.11.5 1.13.4, \
		$(MAKE) module $(api) VERSION=$(version); \
		mv build/modules/ngx_http_51D_module.so modules/ngx_http_51D_$(api)_module-$(version).so;) \
	)

test-full:
	$(MAKE) test TESTS="$(TESTS) tests/nginx-tests"

test:
	if [ ! -f "tests/nginx-tests/lib/Test/Nginx.pm" ]; then \
	rm -r tests/nginx-tests*; \
	cd tests && curl -L -O "http://hg.nginx.org/nginx-tests/archive/tip.tar.gz"; \
	tar zxf tip.tar.gz; \
	mv nginx-tests* nginx-tests; \
	fi;
ifeq (,$(FULLPATH))
	$(error Local binary must be build first (use "make install pattern" or "make install trie"))
else
	TEST_NGINX_BINARY="$(FULLPATH)/nginx" TEST_NGINX_GLOBALS="load_module $(FULLPATH)/build/modules/ngx_http_51D_module.so;" TEST_NGINX_GLOBALS_HTTP="51D_filePath $(FULLPATH)/../data/$(DATAFILE);"  prove -v $(TESTS)
endif