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
Offline processing example of using 51Degrees device detection. The example
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
<li>Open an input file with a list of User-Agents, and an output file,
<p><code>
fin = open('../../data/20000 User Agents.csv', 'r')<br>
fout = open(outputFile, 'w')
</code></p>
<li>Write a header to the output file with the property names in '|'
separated CSV format ('|' sepparated because some User-Agents contain
commas)
<p><code>
fout.write('User-Agent')<br>
for name in properties.split(','):<br>
	fout.write('|' + name)<br>
fout.write('\n')
</code></p>
<li>For the first 20 User-Agents in the input file, performa match then
write the User-Agent along with the values for chosen properties to
the CSV.
<p><code>
while i < 20 :<br>
	userAgent = fin.readline().rstrip('\n')<br>
	fout.write(userAgent)<br>
	device = provider.getMatch(userAgent)<br>
	for name in properties.split(','):<br>
		value = device.getValue(name)<br>
		fout.write('|' + value)<br>
	fout.write('\n')<br>
	i = i + 1
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

dataFile = settings.V3_WRAPPER_DATABASE
properties = settings.PROPERTIES
cacheSize =  settings.CACHE_SIZE
poolSize = settings.POOL_SIZE

inputFile = '../../data/20000 User Agents.csv'
outputFile = 'offlineProcessingOutput.csv'

provider = fiftyone_degrees_mobile_detector_v3_wrapper.Provider(
	dataFile,
	properties,
	cacheSize,
	poolSize)

def output_offline_processing():
	fin = open(inputFile, 'r')
	fout = open(outputFile, 'w')
	fout.write('User-Agent')
	for name in properties.split(','):
		fout.write('|' + name)

	fout.write('\n')
	i = 0
	while i < 20 :
		userAgent = fin.readline().rstrip('\n')
		fout.write(userAgent)
		match = provider.getMatch(userAgent)
		for name in properties.split(','):
			value = match.getValue(name)
			fout.write('|' + value)
		fout.write('\n')
		i += 1
	fin.close()
	fout.close()

def main():
	sys.stdout.write('Starting Offline Processing\n')
	output_offline_processing()
	sys.stdout.write('Output Written to %s\n' % outputFile)
if __name__ == '__main__':
	main()
# // Snippet End
