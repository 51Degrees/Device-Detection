# -*- coding: utf-8 -*-

'''
:copyright: (c) 2015 by 51Degrees.com, see README.md for more details.
:license: MPL2, see LICENSE.txt for more details.
'''

from __future__ import absolute_import
import string
import logging
import pickle
from django.core.validators import validate_ipv46_address
from django.core.exceptions import ValidationError
from django.conf import settings
from fiftyone_degrees import mobile_detector


# Settings names.
SESSION_CACHE = 'FIFTYONE_DEGREES_MOBILE_DETECTOR_SESSION_CACHE'
SESSION_FIELD = 'FIFTYONE_DEGREES_MOBILE_DETECTOR_SESSION_FIELD'

# Default settings values.
DEFAULT_SESSION_CACHE = False
DEFAULT_SESSION_FIELD = '_51degrees_device'


class DetectorMiddleware(object):
    '''Adds lazily generated 'device' attribute to the incoming request.

    '''
    def process_request(self, request):
        request.device = _Device(request)
        return None


class _Device(object):
    '''Proxies lazily generated 'mobile_detector.Device' instance.

    '''
    def __init__(self, request):
        self._request = request
        self._device = None

    def __getattr__(self, name):
        if self._device is None:
            self._device = self._fetch()
        return getattr(self._device, name)

    def _fetch(self):
        # Do *not* break the request when not being able to detect device.
        try:
            if getattr(settings, SESSION_CACHE, DEFAULT_SESSION_CACHE) and \
               hasattr(self._request, 'session'):
                field = getattr(settings, SESSION_FIELD, DEFAULT_SESSION_FIELD)
                if field not in self._request.session:
                    device = self._match()
                    self._request.session[field] = pickle.dumps(device)
                else:
                    device = pickle.loads(self._request.session[field])
            else:
                device = self._match()
        except Exception as e:
            logging.\
                getLogger('fiftyone_degrees.mobile_detector').\
                error('Got an exception while detecting device: %s.' % unicode(e))
            device = mobile_detector.Device()

        # Done!
        return device

    def _match(self):
        # Fetch client IP address.
        client_ip = self._request.META.get('REMOTE_ADDR')
        if 'HTTP_X_FORWARDED_FOR' in self._request.META:
            # HTTP_X_FORWARDED_FOR can be a comma-separated list of IPs.
            # Take just the first valid one (proxies like squid may introduce
            # invalid values like 'unknown' under certain configurations, so
            # a validations is always required).
            for ip in self._request.META['HTTP_X_FORWARDED_FOR'].split(','):
                ip = ip.strip()
                try:
                    validate_ipv46_address(ip)
                    client_ip = ip
                    break
                except ValidationError:
                    pass

        # Fetch HTTP headers.
        # See: https://docs.djangoproject.com/en/dev/ref/request-response/#django.http.HttpRequest.META
        http_headers = {}
        for name, value in self._request.META.iteritems():
            if name in ('CONTENT_LENGTH', 'CONTENT_TYPE',):
                http_headers[self._normalized_header_name(name)] = value
            elif name.startswith('HTTP_'):
                http_headers[self._normalized_header_name(name[5:])] = value

        # Match.
        return mobile_detector.match(http_headers)

    def _normalized_header_name(self, value):
        value = value.replace('_', ' ')
        value = string.capwords(value)
        return value.replace(' ', '-')
