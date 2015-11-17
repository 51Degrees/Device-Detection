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
Match with device id example of using 51Degrees device detection. The example
shows how to:
<ol>
<li>Import settings from the 51Degrees settings file
<p><code>
dataFile = settings.V3_WRAPPER_DATABASE<br>
properties = settings.PROPERTIES<br>
cacheSize = settings.CACHE_SIZE<br>
poolSize = settings.POOL_SIZE<br>
</code></p>
<li>Instantiate the 51Degrees device detection provider with these
properties
<p><code>
provider = fiftyone_degrees_mobile_detector_v3_wrapper.Provider(dataFile,<br>
	properties,<br>
	cacheSize,<br>
	poolSize)
</code></p>
<li>Produce a match for a single device id
<p><code>
match = provider.getMatchForDeviceId(deviceId)
</code><p>
<li>Extract the value of the IsMobile property
<p><code>
match.getValues('IsMobile')
</code></p>
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
<p><code>
51degrees-mobile-detector settings
</p></code>
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

# Device id string of an iPhone mobile device.
mobileDeviceId = "12280-48866-24305-18092"

# Device id string of Firefox Web browser version 41 on dektop.
desktopDeviceId = "15364-21460-53251-18092"

# Device id string of a MediaHub device.
mediaHubDeviceId = "41231-46303-24154-18092"

def main():
	sys.stdout.write('Starting Match With Device Id Example.\n')

# Carries out a match with a mobile device id.
	sys.stdout.write('\nMobile Device Id: %s\n' % mobileDeviceId)
	match = provider.getMatchForDeviceId(mobileDeviceId)
	sys.stdout.write('   IsMobile: %s\n' % match.getValues('IsMobile'))

# Carries out a match with a desktop device id.
	sys.stdout.write('\nDesktop Device Id: %s\n' % desktopDeviceId)
	match = provider.getMatchForDeviceId(desktopDeviceId)
	sys.stdout.write('   IsMobile: %s\n' % match.getValues('IsMobile'))

# Carries out a match with a MediaHub device id.
	sys.stdout.write('\nMediaHub Device Id: %s\n' % mediaHubDeviceId)
	match = provider.getMatchForDeviceId(mediaHubDeviceId)
	sys.stdout.write('   IsMobile: %s\n' % match.getValues('IsMobile'))

if __name__ == '__main__':
	main()
# // Snippet End
