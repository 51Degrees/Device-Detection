51DEGREES_PYTHON_PATTERN_WRAPPER_ROOT=.
SHELL=/bin/bash

build: clean
	@echo
	@echo "> Building..."
	cd "$(51DEGREES_PYTHON_PATTERN_WRAPPER_ROOT)"; python setup.py build

sdist: build
	@echo
	@echo "> Creating Python source distribution package..."
	cd "$(51DEGREES_PYTHON_PATTERN_WRAPPER_ROOT)"; python setup.py sdist

clean:
	rm -rf "$(51DEGREES_PYTHON_PATTERN_WRAPPER_ROOT)"/{build,dist,*.egg-info}
	find "$(51DEGREES_PYTHON_PATTERN_WRAPPER_ROOT)" -name "*.o" | xargs rm -f
