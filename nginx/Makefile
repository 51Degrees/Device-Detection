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

build-pattern: clean
	mkdir -p 51Degrees_module/src/pattern
	mkdir -p 51Degrees_module/src/cityhash
	cp module_conf/pattern_config 51Degrees_module/config
	cp ../src/pattern/51Degrees.c 51Degrees_module/src/pattern/
	cp ../src/pattern/51Degrees.h 51Degrees_module/src/pattern/
	cp -r ../src/cityhash/ 51Degrees_module/src/
	cp ../src/threading.* 51Degrees_module/src/

build-trie: clean
	mkdir -p 51Degrees_module/src/trie
	cp module_conf/trie_config 51Degrees_module/config
	cp ../src/trie/51Degrees.c 51Degrees_module/src/trie/
	cp ../src/trie/51Degrees.h 51Degrees_module/src/trie/

install-pattern: build-pattern
	cd $(CURDIR)/vendor/nginx && \
	./configure \
	--prefix=$(CURDIR)/build \
	--with-ld-opt="-lm" \
	--add-module=$(CURDIR)/51Degrees_module \
	--with-cc-opt="-DFIFTYONEDEGREES_PATTERN -Wno-ignored-qualifiers -Wno-unused-result" \
	--sbin-path=$(CURDIR) \
	--conf-path="pattern.conf"
	cd $(CURDIR)/vendor/nginx && make install
	cp example/pattern.conf build/

install-pattern-debug: build-pattern
	cd $(CURDIR)/vendor/nginx && \
	./configure \
	--prefix=$(CURDIR)/build \
	--with-ld-opt="-lm" \
	--with-debug \
	--add-module=$(CURDIR)/51Degrees_module \
	--with-cc-opt="-DFIFTYONEDEGREES_PATTERN -Wno-ignored-qualifiers -Wno-unused-result -O0 -g" \
	--sbin-path=$(CURDIR) \
	--conf-path="pattern.conf"
	cd $(CURDIR)/vendor/nginx && make install
	cp example/pattern.conf build/

install-trie: build-trie
	cd $(CURDIR)/vendor/nginx && \
	./configure \
	--prefix=$(CURDIR)/build \
	--with-ld-opt="-lm" \
	--add-module=$(CURDIR)/51Degrees_module \
	--with-cc-opt=" -DFIFTYONEDEGREES_TRIE" \
	--sbin-path=$(CURDIR) \
	--conf-path="trie.conf"
	cd $(CURDIR)/vendor/nginx && make install
	cp example/trie.conf build/

install-trie-debug: build-trie
	cd $(CURDIR)/vendor/nginx && \
	./configure \
	--prefix=$(CURDIR)/build \
	--with-ld-opt="-lm" \
	--with-debug \
	--add-module=$(CURDIR)/51Degrees_module \
	--with-cc-opt=" -DFIFTYONEDEGREES_TRIE -O0 -g" \
	--sbin-path=$(CURDIR) \
	--conf-path="trie.conf"
	cd $(CURDIR)/vendor/nginx && make install
	cp example/trie.conf build/
get-source:
	if [ -d "vendor" ]; then rm -r vendor; fi
	mkdir -p vendor/nginx
	curl -L -O "http://nginx.org/download/nginx-1.8.1.tar.gz"
	tar xzf "nginx-1.8.1.tar.gz"
	mv nginx-1.8.1.tar.gz vendor/
	mv -f -u nginx-1.8.1/* vendor/nginx
	rmdir nginx-1.8.1
