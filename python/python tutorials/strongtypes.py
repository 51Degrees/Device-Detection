#!/usr/bin/env python
'''
<tutorial>
<ol>
<li>Imports the detector and settings modules,
<li>Sets the data file, properties, cache size and pool size,
<li>Initializes the provider,
<li>Gets a match for the user agent
<li>Gets the value of supported bearers from the match object as a string.
Notice this only returns the first element,
<li>Gets the value of supported bearers from the match object as a tuple list
(converted from a C++ string vector).
This can now be handled properly within Python
</tutorial>
'''
# // Snippet Start
from FiftyOneDegrees import fiftyone_degrees_mobile_detector_v3_wrapper
from fiftyone_degrees.mobile_detector.conf import settings
dataFile = settings.V3_WRAPPER_DATABASE
properties = 'SupportedBearers'
cacheSize =  settings.CACHE_SIZE
poolSize = settings.POOL_SIZE

provider = fiftyone_degrees_mobile_detector_v3_wrapper.Provider(dataFile, properties, cacheSize, poolSize)

userAgent = 'Mozilla/5.0 (iPhone; CPU iPhone OS 8_1_1 like Mac OS X) AppleWebKit/600.1.4 (KHTML, like Gecko) Version/8.0 Mobile/12B435 Safari/600.1.4'

device = provider.getMatch(userAgent)
print 'get suppoerted bearers as string:'
print ' %s' % device.getValue('SupportedBearers')
print 'get supported bearers as tuple list:'
print ' tuple list:'
print device.getValues('SupportedBearers')
print ' tuple list elements:'
for value in device.getValues('SupportedBearers'):
	print value
# // Snippet End
