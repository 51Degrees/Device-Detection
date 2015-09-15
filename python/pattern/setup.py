'''
51Degrees Mobile Detector (V3 Wrapper)
==================================================

51Degrees Mobile Detector is a Python wrapper of the lite C pattern-based
mobile detection solution by 51Degrees.mobi. Check out http://51degrees.mobi
for a detailed description, extra documentation and other useful information.

:copyright: (c) 2013 by 51Degrees.mobi, see README.rst for more details.
:license: MPL2, see LICENSE.txt for more details.
'''

from __future__ import absolute_import
import os
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
    tmpdir = tempfile.mkdtemp(prefix='51degrees-mobile-detector-v3-pattern-wrapper-install-')
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
    name='51degrees-mobile-detector-v3-wrapper',
    version='3.2.0.0',
    author='51Degrees.com',
    author_email='info@51degrees.com',
    cmdclass={'build_ext': build_ext},
    packages=find_packages(),
    include_package_data=True,
    ext_modules=[
        Extension('_fiftyone_degrees_mobile_detector_v3_wrapper',
            sources=[
                'wrapper.c',
                os.path.join('lib', 'pattern', '51Degrees.c'),
                os.path.join('lib', 'snprintf', 'snprintf.c'),
		os.path.join('lib', 'cityhash', 'city.c'),
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
    description='51Degrees Mobile Detector (Lite C Pattern Wrapper).',
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
