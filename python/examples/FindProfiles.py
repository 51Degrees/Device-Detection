#!/usr/bin/env python
'''
This Source Code Form is copyright of 51Degrees Mobile Experts Limited.
Copyright 2017 51Degrees Mobile Experts Limited, 5 Charlotte Close,
Caversham, Reading, Berkshire, United Kingdom RG4 7BY

This Source Code Form is the subject of the following patents and patent
applications, owned by 51Degrees Mobile Experts Limited of 5 Charlotte
Close, Caversham, Reading, Berkshire, United Kingdom RG4 7BY:
European Patent No. 2871816;
European Patent Application No. 17184134.9;
United States Patent Nos. 9,332,086 and 9,350,823; and
United States Patent Application No. 15/686,066.

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0.

If a copy of the MPL was not distributed with this file, You can obtain
one at http://mozilla.org/MPL/2.0/.

This Source Code Form is "Incompatible With Secondary Licenses", as
defined by the Mozilla Public License, v. 2.0.
'''

'''
<tutorial>
Find profiles example of using 51Degrees device detection. The example
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
<li>Retrieve all profiles from the data set which match the specified
property value pair
<p><pre class="prettyprint lang-py">
profiles = provider.findProfiles("IsMobile", "True")
</pre>
</p>
<li>Search with a list of profiles for another property value pair.
<p><pre class="prettyprint lang-py">
profiles = provider.findProfiles("ScreenPixelsWidth", "1080", profiles)
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

def main():
	print "Starting Find Profiles Example.\n"

	# Retrive all the mobile profiles from the data set.
	profiles = provider.findProfiles("IsMobile", "True")
	print "There are %d mobile profiles in the %s data file." % (profiles.getCount(), provider.getDataSetName())
	profiles = provider.findProfiles("ScreenPixelsWidth", "1080", profiles)
	print "%d of them have a screen width of 1080 pixels." % profiles.getCount()

	# Retrieve all the non-mobile profiles from the data set.
	profiles = provider.findProfiles("IsMobile", "False")
	print "There are %d non-mobile profiles in the %s data file." % (profiles.getCount(), provider.getDataSetName())
	profiles = provider.findProfiles("ScreenPixelsWidth", "1080", profiles)
	print "%d of them have a screen width of 1080 pixels." % profiles.getCount()

if __name__ == '__main__':
	main()
# // Snippet End
