# -*- coding: utf-8 -*-

'''
Default 51Degrees Mobile Detector settings. Override these using the
FIFTYONE_DEGREES_MOBILE_DETECTOR_SETTINGS environment variable. Both file
paths (e.g. '/etc/51Degrees/51degrees-mobile-detector.settings.py') and module
names (e.g. 'myproject.fiftyone_degrees_mobile_settings') are allowed.
If not specified, defaults to '51degrees-mobile-detector.settings.py'
in the current working directory.

Note that when using the mobile detector in a Django project, settings
can also be specified using the FIFTYONE_DEGREES_MOBILE_DETECTOR_SETTINGS
variable in the Django settings file.

:copyright: (c) 2015 by 51Degrees.com, see README.md for more details.
:license: MPL2, see LICENSE.txt for more details.
'''

from __future__ import absolute_import
import os

###############################################################################
## GENERAL SETTINGS.
###############################################################################

# Sets the preferred mobile device detection method. Available options are:
#
#   - 'v3-wrapper': Requires '51degrees-mobile-detector-v3-wrapper' package.
#   - 'v3-trie-wrapper': Requires '51degrees-mobile-detector-v3-trie-wrapper' package.
#
DETECTION_METHOD = 'v3-wrapper'

# List of case-sensitive property names to be fetched on every device detection. Leave empty to
# fetch all available properties.
PROPERTIES = ''
# Your 51Degrees license key. This is required if you want to set up the automatic
# data file updates.
LICENSE = ''

###############################################################################
## TRIE DETECTOR SETTINGS.
###############################################################################

# Location of the database file. If not specified, the trie-based detection
# method will not be available. Download the latest 51Degrees-LiteV3.2.trie
# file from http://github.com/51Degrees/Device-Detection/data/.
# Compare database options at https://51degrees.com/compare-data-options .
V3_TRIE_WRAPPER_DATABASE = os.path.expanduser('~/51Degrees/51Degrees-LiteV3.2.trie')

###############################################################################
## PATTERN DETECTOR SETTINGS.
###############################################################################

# Location of the database file. If not specified, the trie-based detection
# method will not be available. Download the latest 51Degrees-LiteV3.2.dat
# file from http://github.com/51Degrees/Device-Detection/data/.
# Compare database options at https://51degrees.com/compare-data-options .
V3_WRAPPER_DATABASE = os.path.expanduser('~/51Degrees/51Degrees-LiteV3.2.dat')

# Size of cache allocated
CACHE_SIZE = 10000

#Size of pool allocated
POOL_SIZE = 20

###############################################################################
## USAGE SHARER SETTINGS.
###############################################################################

# Indicates if usage data should be shared with 51Degrees.com. We recommended
# leaving this value unchanged to ensure we're improving the performance and
# accuracy of the solution.
USAGE_SHARER_ENABLED = True

# The detail that should be provided relating to new devices.
# Modification not required for most users.
USAGE_SHARER_MAXIMUM_DETAIL = True

# URL to send new device data to.
# Modification not required for most users.
USAGE_SHARER_SUBMISSION_URL = 'https://devices.51degrees.com/new.ashx'

# Data submission timeout (seconds).
USAGE_SHARER_SUBMISSION_TIMEOUT = 10

# Minimum queue length to launch data submission.
USAGE_SHARER_MINIMUM_QUEUE_LENGTH = 50

# Used to detect local devices.
# Modification not required for most users.
USAGE_SHARER_LOCAL_ADDRESSES = (
    '127.0.0.1',
    '0:0:0:0:0:0:0:1',
)

# The content of fields in this list should not be included in the
# request information sent to 51Degrees.
# Modification not required for most users.
USAGE_SHARER_IGNORED_HEADER_FIELD_VALUES = (
    'Referer',
    'cookie',
    'AspFilterSessionId',
    'Akamai-Origin-Hop',
    'Cache-Control',
    'Cneonction',
    'Connection',
    'Content-Filter-Helper',
    'Content-Length',
    'Cookie',
    'Cookie2',
    'Date',
    'Etag',
    'If-Last-Modified',
    'If-Match',
    'If-Modified-Since',
    'If-None-Match',
    'If-Range',
    'If-Unmodified-Since',
    'IMof-dified-Since',
    'INof-ne-Match',
    'Keep-Alive',
    'Max-Forwards',
    'mmd5',
    'nnCoection',
    'Origin',
    'ORIGINAL-REQUEST',
    'Original-Url',
    'Pragma',
    'Proxy-Connection',
    'Range',
    'Referrer',
    'Script-Url',
    'Unless-Modified-Since',
    'URL',
    'UrlID',
    'URLSCAN-ORIGINAL-URL',
    'UVISS-Referer',
    'X-ARR-LOG-ID',
    'X-Cachebuster',
    'X-Discard',
    'X-dotDefender-first-line',
    'X-DRUTT-REQUEST-ID',
    'X-Initial-Url',
    'X-Original-URL',
    'X-PageView',
    'X-REQUEST-URI',
    'X-REWRITE-URL',
    'x-tag',
    'x-up-subno',
    'X-Varnish',
)
