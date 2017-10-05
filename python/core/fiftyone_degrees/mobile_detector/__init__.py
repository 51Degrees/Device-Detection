# -*- coding: utf-8 -*-

'''
:copyright: (c) 2015 by 51Degrees.com, see README.md for more details.
:license: MPL2, see LICENSE.txt for more details.
'''

from __future__ import absolute_import
from abc import ABCMeta
from fiftyone_degrees.mobile_detector.conf import settings
from fiftyone_degrees.mobile_detector import usage

class Device(object):
    '''Simple device wrapper.

    '''
    def __init__(self, method=None):
        self._method = method
        self._properties = {}

    def set_property(self, name, value):
        self._properties[name] = value

    @property
    def method(self):
        return self._method

    @property
    def properties(self):
        return self._properties

    def __getattr__(self, name):
        if name in self._properties:
            return self._properties.get(name)
        else:
            name = name.lower()
            for aname, value in self._properties.iteritems():
                if name == aname.lower():
                    return value
        return None
    def __getstate__(self):
        return self.__dict__

    def __setstate__(self, d):
        self.__dict__.update(d)


class _Matcher(object):
    '''Abstract matcher class.

    '''
    __metaclass__ = ABCMeta
    _METHODS = {}
    _INSTANCES = {}

    @classmethod
    def register(cls, method, klass):
        cls._METHODS[method] = klass

    @classmethod
    def instance(cls, method):
        if method in cls._METHODS:
            if method not in cls._INSTANCES:
                cls._INSTANCES[method] = cls._METHODS[method]()
            return cls._INSTANCES[method]
        else:
            raise Exception(
                'Requested matching method "%s" does not exist. '
                'Available methods are: %s.' %
                (method, ', '.join(cls._METHODS.keys()),))

    def match(self, user_agent, client_ip=None, http_headers=None):
        # If provided, share usage information.
        if client_ip and http_headers:
            usage.UsageSharer.instance().record(client_ip, http_headers)

        # Delegate on specific matcher implementation.
        return self._match(user_agent)

    def _match(self, user_agent):
        raise NotImplementedError('Please implement this method.')


class _V3WrapperMatcher(_Matcher):
    ID = 'v3-wrapper'

    def __init__(self):
        if settings.V3_WRAPPER_DATABASE:
            try:
                # Does the database file exists and is it readable?
                with open(settings.V3_WRAPPER_DATABASE):
                    pass
            except IOError:
                raise Exception(
                    'The provided detection database file (%s) does not '
                    'exist or is not readable. Please, '
                    'check your settings.' % settings.V3_WRAPPER_DATABASE)
            else:
                from FiftyOneDegrees import fiftyone_degrees_mobile_detector_v3_wrapper
                self.provider = fiftyone_degrees_mobile_detector_v3_wrapper.Provider(settings.V3_WRAPPER_DATABASE, settings.PROPERTIES, int(settings.CACHE_SIZE), int(settings.POOL_SIZE))
        else:
            raise Exception(
                'Trie-based detection method depends on an external '
                'database file. Please, check your settings.')

    def _match(self, user_agent):
        # Delegate on wrapped implementation.
        returnedMatch = None
        try:
            returnedMatch = self.provider.getMatch(user_agent)
        except Exception as e:
            settings.logger.error(
                'Got exception while matching user agent string "%s": %s.'
                % (user_agent, unicode(e),))

        # Pythonize result.
        result = Device(self.ID)

        if returnedMatch:
                result.set_property('Id', returnedMatch.getDeviceId())
                result.set_property('MatchMethod', returnedMatch.getMethod())
                result.set_property('Difference', returnedMatch.getDifference())
                result.set_property('Rank', returnedMatch.getRank())
                if settings.PROPERTIES == '':
                        for key in self.provider.getAvailableProperties():
                                value = returnedMatch.getValues(key)
                                if value:
                                        result.set_property(key, ' '.join(value))
                                else:
                                        result.set_property(key, 'N/A in Lite')
                else:
                        for key in settings.PROPERTIES.split(','):
                                value = returnedMatch.getValues(key)
                                if value:
                                        result.set_property(key, ' '.join(value))

        # Done!
        return result



class _V3TrieWrapperMatcher(_Matcher):
    ID = 'v3-trie-wrapper'

    def __init__(self):
        if settings.V3_TRIE_WRAPPER_DATABASE:
            try:
                # Does the database file exists and is it readable?
                with open(settings.V3_TRIE_WRAPPER_DATABASE):
                    pass
            except IOError:
                raise Exception(
                    'The provided detection database file (%s) does not '
                    'exist or is not readable. Please, '
                    'check your settings.' % settings.V3_TRIE_WRAPPER_DATABASE)
            else:
                from FiftyOneDegrees import fiftyone_degrees_mobile_detector_v3_trie_wrapper
                self.provider = fiftyone_degrees_mobile_detector_v3_trie_wrapper.Provider(settings.V3_TRIE_WRAPPER_DATABASE, settings.PROPERTIES)
        else:
            raise Exception(
                'Trie-based detection method depends on an external '
                'database file. Please, check your settings.')

    def _match(self, user_agent):
        # Delegate on wrapped implementation.
        returnedMatch = None
        try:
            returnedMatch = self.provider.getMatch(user_agent)
        except Exception as e:
            settings.logger.error(
                'Got exception while matching user agent string "%s": %s.'
                % (user_agent, unicode(e),))

        # Pythonize result.
        result = Device(self.ID)

        print settings.PROPERTIES
        if returnedMatch:
                if settings.PROPERTIES == '':
                        for key in self.provider.getAvailableProperties():
                                value = returnedMatch.getValues(key)
                                if value:
                                        result.set_property(key, ' '.join(value))
                                else:
                                        result.set_property(key, 'N/A in Lite')
                else:
                        for key in settings.PROPERTIES.split(','):
                                value = returnedMatch.getValues(key)
                                if value:
                                        result.set_property(key, ' '.join(value))

        # Done!
        return result




# Register matching methods.
for klass in [_V3WrapperMatcher, _V3TrieWrapperMatcher]:
    _Matcher.register(klass.ID, klass)


def match(user_agent, client_ip=None, http_headers=None, method=None):
    '''Fetches device data based on an user agent string.

        *user_agent* is an user agent string.

        *client_ip* is a string with the client IP address (optional). If provided
        it'll will be submitted to 51Degrees.mobi in order to improve performance
        and accuracy of further device detections.

        *http_headers* is a dictionary containing all HTTP headers (optional).
        If provided, it'll will be submitted (removing confidential data such as
        cookies) to 51Degrees.mobi in order to improve performance and accuracy
        of further device detections.

        *method* is a string with the desired device detection method. If not
        specified, settings.DETECTION_METHOD will be used.

        Returns Device instance.
    '''
    # Fetch matcher instance.
    matcher = _Matcher.instance(
        method
        if method is not None
        else settings.DETECTION_METHOD)

    # Match!
    return matcher.match(user_agent, client_ip, http_headers)

