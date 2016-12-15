# -*- coding: utf-8 -*-

'''
Settings and configuration for 51Degrees Mobile Detector.

Values will be read from the file or module specified by the
FIFTYONE_DEGREES_MOBILE_DETECTOR_SETTINGS environment variable. Both file
paths (e.g. '/etc/51degrees-mobile-detector.settings.py') and module
names (e.g. 'myproject.fiftyone_degrees_mobile_settings') are allowed.
If not specified, defaults to '51degrees-mobile-detector.settings.py'
in the current working directory.

Additionally, when using the mobile detector in a Django project, settings
can also be specified using the FIFTYONE_DEGREES_MOBILE_DETECTOR_SETTINGS
variable in the Django settings file.

:copyright: (c) 2015 by 51Degrees.com, see README.md for more details.
:license: MPL2, see LICENSE.txt for more details.
'''

from __future__ import absolute_import
import os
import sys
import imp
import logging
from fiftyone_degrees.mobile_detector.conf import default


class _Settings(object):
    VERSION = '3.2'
    try:
        import pkg_resources
        VERSION = pkg_resources.get_distribution('51degrees-mobile-detector').version
    except:
        pass

    def __init__(self, settings_file_or_module):
        # Add default settings.
        self._add_settings(default)

        # Try to load settings from file/module pointed by the
        # environment variable.
        try:
            __import__(settings_file_or_module)
            self._add_settings(sys.modules[settings_file_or_module])
        except:
            try:
                self._add_settings(imp.load_source(
                    'fiftyone_degrees.conf._file',
                    settings_file_or_module))
            except:
                pass

        # Try to load setting from the Django settings file.
        try:
            from django.conf import settings
            from django.core import exceptions
            try:
                for name, value in getattr(settings, 'FIFTYONE_DEGREES_MOBILE_DETECTOR_SETTINGS', {}).iteritems():
                    self._add_setting(name, value)
            except exceptions.ImproperlyConfigured:
                pass
        except ImportError:
            pass

        # Add logger instance.
        self.logger = logging.getLogger('fiftyone_degrees.mobile_detector')

    def _add_settings(self, mod):
        '''Updates this dict with mod settings (only ALL_CAPS).

        '''
        for name in dir(mod):
            if name == name.upper():
                self._add_setting(name, getattr(mod, name))

    def _add_setting(self, name, value):
        '''Updates this dict with a specific setting.

        '''
        if name == 'USAGE_SHARER_IGNORED_HEADER_FIELD_VALUES':
            value = tuple([item.upper() for item in value])
        setattr(self, name, value)

settings = _Settings(
    os.environ.get(
        'FIFTYONE_DEGREES_MOBILE_DETECTOR_SETTINGS',
        os.path.join(os.getcwd(), '51degrees-mobile-detector.settings.py')))
