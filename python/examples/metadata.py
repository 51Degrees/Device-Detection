#!/usr/bin/env python
'''
<turorial>
<ol>
<li>Imports the detector and settings modules,
<li>Sets the data file, properties, cache size and pool size,
<li>Initialises the provider,
<li>Lists the meta data fetched from the data set.
</ol>
Note: this does not need a match to be carried out as it is fethching information about the data set.
<br>
</turorial>
'''
#// Snippet Start
from FiftyOneDegrees import fiftyone_degrees_mobile_detector_v3_wrapper
from fiftyone_degrees.mobile_detector.conf import settings
dataFile = settings.V3_WRAPPER_DATABASE
properties = 'BrowserName,BrowserVendor,BrowserVersion,DeviceType,HardwareVendor,
IsTablet,IsMobile,IsCrawler,ScreenInchesDiagonal,ScreenPixelsWidth'
cacheSize = settings.CACHE_SIZE
poolSize = settings.POOL_SIZE

provider = fiftyone_degrees_mobile_detector_v3_wrapper.Provider(dataFile, properties, cacheSize, poolSize)

print 'Data Set Information\n'
print 'Name: %s\n' % provider.getDataSetName()
print 'Published: %s\n' % provider.getDataSetPublishedDate()
print 'Next Update Date: %s\n' % provider.getDataSetNextUpdateDate()
print 'Signature Count: %d\n' % provider.getDataSetSignatureCount()
print 'Device Combinations: %d\n' % provider.getDataSetDeviceCombinations()