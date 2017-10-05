# -*- coding: utf-8 -*-

'''
:copyright: (c) 2015 by 51Degrees.com, see README.md for more details.
:license: MPL2, see LICENSE.txt for more details.
'''

from __future__ import absolute_import
from datetime import datetime
import gzip
import urllib2
import threading
try:
    import xml.etree.cElementTree as ET
except ImportError:
    import xml.etree.ElementTree as ET
try:
    import cStringIO as StringIO
except ImportError:
    import StringIO
from fiftyone_degrees.mobile_detector.conf import settings


class UsageSharer(threading.Thread):
    '''Class used to record usage information.

    Records & submits usage information which is essential to ensuring
    51Degrees.mobi is optimized for performance and accuracy for current
    devices on the web.

    '''
    # Singleton reference.
    _instance = None

    def __init__(self):
        super(UsageSharer, self).__init__()

        # Check singleton instance.
        if self._instance is not None:
            raise ValueError('An instance of UsageSharer class already exists.')

        # Init internal singleton state.
        self._queue = []
        self._stopping = False
        self._event = threading.Event()

    @classmethod
    def instance(cls):
        '''Returns singleton UsageSharer instance.

        '''
        if cls._instance is None:
            cls._instance = cls()
        return cls._instance

    def record(self, client_ip, http_headers):
        '''Adds request details.

        Adds some request details to the queue for further submission by
        the background thread.

        *client_ip* is a string with the client IP address.

        *http_headers* is a dictionary containing all HTTP headers.

        '''
        # Enabled?
        if settings.USAGE_SHARER_ENABLED and self._stopping == False:
            # Launch background daemon data submission thread if not running.
            if not self.is_alive():
                self.daemon = True
                self.start()

            # Add the request details to the queue for further submission.
            self._queue.append(self._get_item(client_ip, http_headers))

            # Signal the background thread to check if it should send
            # queued data.
            self._event.set()

    def stop(self):
        '''Gracefully stops background data submission thread.

        '''
        if self.is_alive():
            settings.logger.info('Stopping 51Degrees UsageSharer.')
            self._stopping = True
            self._event.set()
            self.join()

    def run(self):
        '''Runs the background daemon data submission thread.

        Used to send the devices data back to 51Degrees.mobi after the
        minimum queue length has been reached.

        '''
        # Log.
        settings.logger.info('Starting 51Degrees UsageSharer.')

        # Submission loop.
        while not self._stopping:
            # Wait while event's flag is set to True.
            while not self._event.is_set():
                self._event.wait()

            # If there are enough items in the queue, or the thread is being
            # stopped, submit the queued data.
            length = len(self._queue)
            if length >= settings.USAGE_SHARER_MINIMUM_QUEUE_LENGTH or (length > 0 and self._stopping):
                self._submit()

            # Reset the internal event's flag to False.
            self._event.clear()

        # Log.
        settings.logger.info('Stopped 51Degrees UsageSharer.')

    def _is_local(self, address):
        return address in settings.USAGE_SHARER_LOCAL_ADDRESSES

    def _get_item(self, client_ip, http_headers):
        # Create base device element.
        device = ET.Element('Device')

        # Add the current date and time.
        item = ET.SubElement(device, 'DateSent')
        item.text = datetime.utcnow().replace(microsecond=0).isoformat()

        # Add product name and version.
        item = ET.SubElement(device, 'Version')
        item.text = settings.VERSION
        item = ET.SubElement(device, 'Product')
        item.text = 'Python Mobile Detector'

        # Add client IP address (if is not local).
        if not self._is_local(client_ip):
            item = ET.SubElement(device, 'ClientIP')
            item.text = client_ip

        # Filter & add HTTP headers.
        for name, value in http_headers.iteritems():
            # Determine if the field should be treated as a blank.
            blank = name.upper() in settings.USAGE_SHARER_IGNORED_HEADER_FIELD_VALUES

            # Include all header values if maximum detail is enabled, or
            # header values related to the user agent or any header
            # key containing profile or information helpful to determining
            # mobile devices.
            if settings.USAGE_SHARER_MAXIMUM_DETAIL or \
               name.upper() in ('USER-AGENT', 'HOST', 'PROFILE') or \
               blank:
                item = ET.SubElement(device, 'Header')
                item.set('Name', name)
                if not blank:
                    item.text = unicode(value)

        # Done!
        return device

    def _submit(self):
        '''Sends all the data on the queue.

        '''
        settings.logger.info('Submitting UsageSharer queued data to %s.' % settings.USAGE_SHARER_SUBMISSION_URL)

        # Build output stream.
        stream = StringIO.StringIO()
        gzStream = StringIO.StringIO()
        devices = ET.Element('Devices')
        while len(self._queue) > 0:
            devices.append(self._queue.pop())
        ET.ElementTree(devices).write(
            stream,
            encoding='utf8',
            xml_declaration=True)
        stream.seek(0,0)
        # Gzip the data.
        with gzip.GzipFile(fileobj=gzStream, mode='wb') as gzObj:
            gzObj.write(stream.read())
        gzStream.seek(0,0)
        # Submit gzipped data.
        request = urllib2.Request(
            url=settings.USAGE_SHARER_SUBMISSION_URL,
            data=gzStream.read(),
            headers={
                'Content-Type': 'text/xml; charset=utf-8',
                'Content-Encoding': 'gzip',
            })
        try:
            response = urllib2.urlopen(request, timeout=settings.USAGE_SHARER_SUBMISSION_TIMEOUT)
        except:
            # Turn off functionality.
            self._stopping = True
        else:
            # Get the response and record the content if it's valid. If it's
            # not valid consider turning off the functionality.
            code = response.getcode()
            if code == 200:
                # OK. Do nothing.
                pass
            elif code == 408:
                # Request Timeout. Could be temporary, do nothing.
                pass
            else:
                # Turn off functionality.
                self._stopping = True
