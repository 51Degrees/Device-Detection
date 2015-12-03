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
Getting started example of using 51Degrees device detection match metrics 
information. The example shows how to:
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
<li>Obtain device Id: consists of four components separated by a hyphen 
symbol: Hardware-Platform-Browser-IsCrawler where each Component 
represents an ID of the corresponding Profile.
<p><pre class="prettyprint lang-py">match.getDeviceId()</pre>
<li>Obtain match method: provides information about the 
algorithm that was used to perform detection for a particular User-Agent. 
For more information on what each method means please see: 
<a href="https://51degrees.com/support/documentation/pattern">
How device detection works</a>
<p><pre class="prettyprint lang-py">match.getMethod()</pre>
<li>Obtain difference:  used when detection method is not Exact or None. 
This is an integer value and the larger the value the less confident the 
detector is in this result.
<p><pre class="prettyprint lang-py">match.getDifference()</pre>
<li>Obtain signature rank: an integer value that indicates how popular 
the device is. The lower the rank the more popular the signature.
<p><pre class="prettyprint lang-py">match.getRank()</pre>
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

'''
Imports settings from the settings file. The Default settings file, and
details on how to change it can be output by running the command
<p><pre class="prettyprint lang-py">
51degrees-mobile-detector settings
</p></pre>
'''
dataFile = settings.V3_WRAPPER_DATABASE
properties = settings.PROPERTIES
cacheSize = settings.CACHE_SIZE
poolSize = settings.POOL_SIZE

'''
Initialises the device detection provider with settings from the settings
file. By default this will use the included Lite data file For more info
see:
<a href="https://51degrees.com/compare-data-options">compare data options
</a>
'''
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
output_metrics function. Takes a match object as an argument and
prints the match metrics relating to the specific match.
'''
def output_metrics(match):
	sys.stdout.write('   Id: %s\n' % match.getDeviceId())
	sys.stdout.write('   Match Method: %s\n' % match.getMethod())
	sys.stdout.write('   Difference: %s\n' % match.getDifference())
	sys.stdout.write('   Rank: %s\n' % match.getRank())

def main():
	sys.stdout.write('Starting Getting Started Match Metrics Example.\n')

# Carries out a match with a mobile User-Agent.
	sys.stdout.write('Mobile User-Agent: %s\n' % mobileUserAgent)
	match = provider.getMatch(mobileUserAgent)
	output_metrics(match)
# Carries out a match with a desktop User-Agent.
	sys.stdout.write('Desktop User-Agent: %s\n' % desktopUserAgent)
	match = provider.getMatch(desktopUserAgent)
	output_metrics(match)
# Carries out a match with a MediaHub User-Agent.
	sys.stdout.write('Media Hub User-Agent: %s\n' % mediaHubUserAgent)
	match = provider.getMatch(mediaHubUserAgent)
	output_metrics(match)

if __name__ == '__main__':
	main()
# // Snippet End
