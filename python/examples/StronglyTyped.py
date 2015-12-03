#!/usr/bin/env python
'''
This Source Code Form is copyright of 51Degrees Mobile Experts Limited.
Copyright (c) 2015 51Degrees Mobile Experts Limited, 5 Charlotte Close,
Caversham, Reading, Berkshire, United Kingdom RG4 7BY

This Source Code Form is the subject of the following patent
applications, owned by 51Degrees Mobile Experts Limited of 5 Charlotte
Close, Caversham, Reading, Berkshire, United Kingdom RG4 7BY:
European Patent Application No. 13192291.6; and
United States Patent Application Nos. 14/085,223 and 14/085,301.

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0.

If a copy of the MPL was not distributed with this file, You can obtain
one at http://mozilla.org/MPL/2.0/.

This Source Code Form is "Incompatible With Secondary Licenses", as
defined by the Mozilla Public License, v. 2.0.
'''

'''
<tutorial>
Getting started example of using 51Degrees device detection. The example
shows how to:
<ol>
<li>Import settings from the 51Degrees settings file
<p><pre class="prettyprint lang-py">
dataFile = settings.V3_WRAPPER_DATABASE
properties = settings.PROPERTIES
cacheSize = settings.CACHE_SIZE
poolSize = settings.POOL_SIZE
</pre></p>
<li>Instantiate the 51Degrees device detection provider with these
properties
<p><pre class="prettyprint lang-py">
provider = fiftyone_degrees_mobile_detector_v3_wrapper.Provider(dataFile,
	properties,
	cacheSize,
	poolSize)
</pre></p>
<li>Produce a match for a single HTTP User-Agent header
<p><pre class="prettyprint lang-py">
match = provider.getMatch(userAgent)
</pre><p>
<li>Extract the value of the IsMobile property as boolean
<p><pre class="prettyprint lang-py">
def isMobile(userAgent):
	match = provider.getMatch(userAgent)
	if (match.getValue('IsMobile') == 'True'):
		return True

</pre></p>
</ol>
This example can be run in any directory, but assumes your
settings file contains a valid dataFile location and has the
IsMobile property selected.
</tutorial>
'''
# // Snippet Start
from FiftyOneDegrees import fiftyone_degrees_mobile_detector_v3_wrapper
from fiftyone_degrees.mobile_detector.conf import settings
import sys

dataFile = settings.V3_WRAPPER_DATABASE
properties = settings.PROPERTIES
cacheSize =  settings.CACHE_SIZE
poolSize = settings.POOL_SIZE

provider = fiftyone_degrees_mobile_detector_v3_wrapper.Provider(dataFile,
	properties,
	cacheSize,
	poolSize)

# User-Agent string of an iPhone mobile device.
mobileUserAgent = ("Mozilla/5.0 (iPhone; CPU iPhone OS 7_1 like Mac OS X) "
"AppleWebKit/537.51.2 (KHTML, like Gecko) 'Version/7.0 Mobile/11D167 "
"Safari/9537.53")

# User-Agent string of Firefox Web browser version 41 on desktop.
desktopUserAgent = ("Mozilla/5.0 (Windows NT 6.3; WOW64; rv:41.0) "
"Gecko/20100101 Firefox/41.0")

# User-Agent string of a MediaHub device.
mediaHubUserAgent = ("Mozilla/5.0 (Linux; Android 4.4.2; X7 Quad Core "
"Build/KOT49H) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 "
"Chrome/30.0.0.0 Safari/537.36")

'''
isMobile function. Takes a User-Agent as an argument, carries out a
match and returns a boolean value for the IsMobile property of the
matched device.
'''
def isMobile(userAgent):
	match = provider.getMatch(userAgent)
	if (match.getValue('IsMobile') == 'True'):
		return True

def main():
	sys.stdout.write('Mobile User-Agent: %s\n' % mobileUserAgent)

# Determines whether the mobile User-Agent is a mobile device.
	if isMobile(mobileUserAgent):
		sys.stdout.write('   Mobile\n')
	else:
		sys.stdout.write('   Non-Mobile\n')

# Determines whether the desktop User-Agent is a mobile device.
	sys.stdout.write('Desktop User-Agent: %s\n' % desktopUserAgent)
	if isMobile(desktopUserAgent):
		sys.stdout.write('   Mobile\n')
	else:
		sys.stdout.write('   Non-Mobile\n')

# Determines whether the MediaHub User-Agent is a mobile device.
	sys.stdout.write('Media Hub User-Agent: %s\n' % mediaHubUserAgent)
	if isMobile(mediaHubUserAgent):
		sys.stdout.write('   Mobile\n')
	else:
		sys.stdout.write('   Non-Mobile\n')

if __name__ == '__main__':
	main()
# // Snippet End
