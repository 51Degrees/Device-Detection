'''
51Degrees Mobile Detector (V3 Trie Wrapper)
==================================================

51Degrees Mobile Detector is a Python wrapper of the C trie-based
mobile detection solution by 51Degrees.com. Check out http://51degrees.com
for a detailed description, extra documentation and other useful information.

:copyright: (c) 2015 by 51Degrees.com, see README.md for more details.
:license: MPL2, see LICENSE.txt for more details.
'''

from __future__ import absolute_import
import os
import sys
import subprocess
import shutil
import tempfile
from setuptools import setup, find_packages, Extension
from distutils.command.build_ext import build_ext as _build_ext
from distutils import ccompiler

def has_snprintf():
    '''Checks C function snprintf() is available in the platform.

    '''
    cc = ccompiler.new_compiler()
    tmpdir = tempfile.mkdtemp(prefix='51degrees-mobile-detector-v3-trie-wrapper-install-')
    try:
        try:
            source = os.path.join(tmpdir, 'snprintf.c')
            with open(source, 'w') as f:
                f.write(
                    '#include <stdio.h>\n'
                    'int main() {\n'
                    '  char buffer[8];\n'
                    '  snprintf(buffer, 8, "Hey!");\n'
                    '  return 0;\n'
                    '}')
            objects = cc.compile([source], output_dir=tmpdir)
            cc.link_executable(objects, os.path.join(tmpdir, 'a.out'))
        except:
            return False
        return True
    finally:
        shutil.rmtree(tmpdir)


class build_ext(_build_ext):
    def run(self, *args, **kwargs):
        return _build_ext.run(self, *args, **kwargs)

define_macros = []
if has_snprintf():
    define_macros.append(('HAVE_SNPRINTF', None))
setup(
    name='51degrees-mobile-detector-v3-trie-wrapper',
    version='3.2.5.3',
    author='51Degrees.com',
    author_email='info@51degrees.com',
    cmdclass={'build_ext': build_ext},
    packages=find_packages(),
    include_package_data=True,
    data_files=[(os.path.expanduser('~/51Degrees'), ['data/51Degrees-LiteV3.2.trie'])],
    ext_modules=[
        Extension('_fiftyone_degrees_mobile_detector_v3_trie_wrapper',
            sources=[
		'src/trie/51Degrees.c',
		'src/cityhash/city.c',
                'src/trie/51Degrees_python.cxx',
		'src/trie/Provider.cpp',
		'src/trie/Match.cpp',
            ],
            define_macros=define_macros,
            extra_compile_args=[
                '-w',
                # Let the linker strip duplicated symbols (required in OSX).
                '-fcommon',
                # Avoid 'Symbol not found' errors on extension load caused by
                # usage of vendor specific '__inline' keyword.
                '-std=gnu89',
            ],
        ),
    ],
    url='http://51degrees.com',
    description='51Degrees Mobile Detector (Lite C Trie Wrapper).',
    long_description=__doc__,
    license='MPL2',
    classifiers=[
        'Development Status :: 5 - Production/Stable',
        'Intended Audience :: Developers',
        'Topic :: Software Development :: Libraries',
        'License :: OSI Approved :: Mozilla Public License 2.0 (MPL 2.0)',
        'Programming Language :: C',
        'Programming Language :: Python',
        'Programming Language :: Python :: 2',
        'Programming Language :: Python :: 2.6',
        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.2',
        'Programming Language :: Python :: 3.3',
        'Operating System :: POSIX',
        'Operating System :: MacOS :: MacOS X',
    ],
    install_requires=[
        'distribute',
	'51degrees-mobile-detector',
    ],
)
