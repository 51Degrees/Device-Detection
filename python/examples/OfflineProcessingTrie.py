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
Offline processing example of using 51Degrees device detection. The example
shows how to:
<ol>
<li>Import settings from the 51Degrees settings file
<p><pre class="prettyprint lang-py">
dataFile = settings.V3_TRIE_WRAPPER_DATABASE
properties = settings.PROPERTIES
</pre></p>
<li>Instantiate the 51Degrees device detection provider with these
properties
<p><pre class="prettyprint lang-py">
provider = fiftyone_degrees_mobile_detector_v3_trie_wrapper.Provider(dataFile,
	properties)
</pre></p>
<li>Open an input file with a list of User-Agents, and an output file,
<p><pre class="prettyprint lang-py">
fin = open(inputFile, 'r')
fout = open(outputFile, 'w')
</pre></p>
<li>Write a header to the output file with the property names in '|'
separated CSV format ('|' separated because some User-Agents contain
commas)
<p><pre class="prettyprint lang-py">
fout.write('User-Agent')
for name in properties.split(','):
	fout.write('|' + name)
fout.write('\n')
</pre></p>
<li>For the first 20 User-Agents in the input file, perform a match then
write the User-Agent along with the values for chosen properties to
the CSV.
<p><pre class="prettyprint lang-py">
while i < 20 :
	userAgent = fin.readline().rstrip('\n')
	fout.write(userAgent)
	device = provider.getMatch(userAgent)
	for name in properties.split(','):
		value = device.getValue(name)
		fout.write('|' + value)
	fout.write('\n')
	i = i + 1
</pre></p>
</ol>
This example can be run in any directory, but assumes your
settings file contains a valid dataFile location and has the
IsMobile property selected.
</tutorial>
'''
# // Snippet Start
from FiftyOneDegrees import fiftyone_degrees_mobile_detector_v3_trie_wrapper
from fiftyone_degrees.mobile_detector.conf import settings
import sys

'''
Imports settings from the settings file. The Default settings file, and
details on how to change it can be output by running the command
<p><pre class="prettyprint lang-py">
51degrees-mobile-detector settings
</p></pre>
'''
dataFile = settings.V3_TRIE_WRAPPER_DATABASE
properties = settings.PROPERTIES

inputFile = '../../data/20000 User Agents.csv'
outputFile = 'offlineProcessingOutput.csv'

'''
Initialises the device detection provider with settings from the settings
file. By default this will use the included Lite data file For more info
see:
<a href="https://51degrees.com/compare-data-options">compare data options
</a>
'''
provider = fiftyone_degrees_mobile_detector_v3_trie_wrapper.Provider(
	dataFile,
	properties)

# Carries out match for first 20 User-Agents and prints results to
# output file.
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
