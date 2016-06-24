51DEGREES_PYTHON_PATTERN_WRAPPER_ROOT=.
SHELL=/bin/bash

build: clean
	@echo
	@echo "> Building..."
	cd "$(51DEGREES_PYTHON_PATTERN_WRAPPER_ROOT)"; mkdir src
	mkdir data
	cp -r ../../src/trie src/trie
	cp -r ../../src/cityhash src/cityhash
	cp ../../src/threading.* src/
	cp ../../data/51Degrees-LiteV3.2.trie data/
	swig -python -c++ -module fiftyone_degrees_mobile_detector_v3_trie_wrapper -outdir FiftyOneDegrees -o src/trie/51Degrees_python.cxx src/trie/51Degrees.i

sdist: build
	@echo
	@echo "> Creating Python source distribution package..."
	cd "$(51DEGREES_PYTHON_PATTERN_WRAPPER_ROOT)"; python setup.py sdist

clean:
	rm -rf "$(51DEGREES_PYTHON_PATTERN_WRAPPER_ROOT)"/{build,dist,*.egg-info,src,data}
	find "$(51DEGREES_PYTHON_PATTERN_WRAPPER_ROOT)" -name "*.o" | xargs rm -f

install: sdist
	pip install --verbose --upgrade dist/*
	make clean
