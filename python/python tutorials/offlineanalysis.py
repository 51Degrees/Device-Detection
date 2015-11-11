#!/usr/bin/env python
'''
<tutorial>
<ol>
<li>Imports the detector and settings modules,
<li>Sets the data file, properties, cache size and pool size,
<li>Initializes the provider,
<li>Opens an input file with a list of User-Agents, and an output file,
<li>Writes a header to the output file with the property names in '|'
separated CSV format
<li>For the first 20 User-Agents in the input file, a match is performed and
the User-Agent along with the values for chosen properties are output to
the CSV.
</ol>
</tutorial>
'''
# // Snippet Start
from FiftyOneDegrees import fiftyone_degrees_mobile_detector_v3_wrapper
from fiftyone_degrees.mobile_detector.conf import settings
import sys

dataFile = settings.V3_WRAPPER_DATABASE
properties = 'IsMobile,PlatformName,PlatformVersion'
cacheSize =  settings.CACHE_SIZE
poolSize = settings.POOL_SIZE

outputFile = 'offlineProcessingOutput.csv'

provider = fiftyone_degrees_mobile_detector_v3_wrapper.Provider(
	dataFile,
	properties,
	cacheSize,
	poolSize)

def output_offline_processing():
	fin = open('../../data/20000 User Agents.csv', 'r')
	fout = open(outputFile, 'w')
	fout.write('User-Agent')
	for name in properties.split(','):
		fout.write('|' + name)

	fout.write('\n')
	i = 0
	while i < 20 :
		userAgent = fin.readline().rstrip('\n')
		fout.write(userAgent)
		device = provider.getMatch(userAgent)
		for name in properties.split(','):
			value = device.getValue(name)
			fout.write('|' + value)
		fout.write('\n')
		i = i + 1
	fin.close()
	fout.close()

def main():
	sys.stdout.write('Stating Offline Processing\n')
	output_offline_processing()
	sys.stdout.write('Output Writen to %s\n' % outputFile)
if __name__ == '__main__':
	main()
# // Snippet End
