51DEGREES_PYTHON_TRIE_WRAPPER_ROOT=.
SHELL=/bin/bash

build: clean
	@echo
	@echo "> Building..."
	cd "$(51DEGREES_PYTHON_TRIE_WRAPPER_ROOT)"; mkdir src
	mkdir data
	cp -r ../../src/pattern src/pattern
	cp -r ../../src/cityhash src/cityhash
	cp ../../src/threading.* src/
	cp ../../data/51Degrees-LiteV3.2.dat data/
	swig -python -c++ -module fiftyone_degrees_mobile_detector_v3_wrapper -outdir FiftyOneDegrees -o src/pattern/51Degrees_python.cxx src/pattern/51Degrees.i

sdist: build
	@echo
	@echo "> Creating Python source distribution package..."
	cd "$(51DEGREES_PYTHON_TRIE_WRAPPER_ROOT)"; python setup.py sdist

clean:
	rm -rf "$(51DEGREES_PYTHON_TRIE_WRAPPER_ROOT)"/{build,dist,*.egg-info,src,data}
	find "$(51DEGREES_PYTHON_TRIE_WRAPPER_ROOT)" -name "*.o" | xargs rm -f

install: sdist
	pip install --verbose --upgrade dist/*
	make clean
