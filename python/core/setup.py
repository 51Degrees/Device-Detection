'''
51Degrees Mobile Detector
=========================

51Degrees Mobile Detector is a server side mobile detection solution
by 51Degrees. Check out http://51degrees.com for a detailed
description, extra documentation and other useful information.

:copyright: (c) 2015 by 51Degrees, see README.md for more details.
:license: MPL2, see LICENSE.txt for more details.
'''

from __future__ import absolute_import
import sys
from setuptools import setup, find_packages

extra = {}

# Minimum Python version.
if sys.version_info < (2, 6):
    raise Exception('51Degrees Mobile Detector requires Python 2.6 or higher.')

# Python 3.
if sys.version_info[0] == 3:
    extra.update(use_2to3=True)

setup(
    name='51degrees-mobile-detector',
    version='3.2.11.6',
    author='51Degrees',
    author_email='info@51degrees.com',
    packages=find_packages(),
    include_package_data=True,
    url='http://51degrees.com',
    description='51Degrees Mobile Detector.',
    long_description=__doc__,
    license='MPL2',
    entry_points={
        'console_scripts': [
            '51degrees-mobile-detector = fiftyone_degrees.mobile_detector.runner:main',
        ],
    },
    classifiers=[
        'Development Status :: 5 - Production/Stable',
        'Intended Audience :: Developers',
        'Topic :: Software Development :: Libraries',
        'License :: OSI Approved :: Mozilla Public License 2.0 (MPL 2.0)',
        'Programming Language :: Python',
        'Programming Language :: Python :: 2',
        'Programming Language :: Python :: 2.6',
        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.2',
        'Programming Language :: Python :: 3.3',
        'Operating System :: OS Independent',
        'Operating System :: POSIX',
        'Operating System :: Microsoft :: Windows',
        'Operating System :: MacOS :: MacOS X',
    ],
    install_requires=[
        'distribute',
    ],
    **extra
)
