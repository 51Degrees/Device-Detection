#!/usr/bin/env python

'''
:copyright: (c) 2015 by 51Degrees.com, see README.md for more details.
:license: MPL2, see LICENSE.txt for more details.
'''

from FiftyOneDegrees import fiftyone_degrees_mobile_detector_v3_wrapper
import os
import sys
import subprocess

#######################################################
# Set the match perameters
#######################################################

dataFile = os.path.expanduser('~/51Degrees/51Degrees-LiteV3.2.dat')
properties = 'BrowserName,BrowserVendor,BrowserVersion,DeviceType,HardwareVendor,IsTablet,IsMobile,IsCrawler,ScreenInchesDiagonal,ScreenPixelsWidth'
cacheSize = 10000
poolSize = 20

#######################################################
# Initialize the provider
#######################################################

provider = fiftyone_degrees_mobile_detector_v3_wrapper.Provider(dataFile, properties, cacheSize, poolSize)

#######################################################
# Set User-Agent and http headers
#######################################################

userAgent = 'Mozilla/5.0 (Linux; Android 4.4.2; en-us; SAMSUNG SCH-I545 Build/KOT49H) AppleWebKit/537.36 (KHTML, like Gecko) Version/1.5 Chrome/28.0.1500.94 Mobile Safari/537.36'
httpHeaders = fiftyone_degrees_mobile_detector_v3_wrapper.MapStringString()
httpHeaders.__setitem__('User-Agent', 'Opera/9.52 (X11; Linux i686; U; en)')
httpHeaders.__setitem__('Device-Stock-UA', userAgent)
httpHeaders.__setitem__("Accept-Types", "*/*")

#######################################################
# Returns match from single User-Agent
#######################################################
def match():
	device = provider.getMatch(userAgent)
	outputProperties(device)

#######################################################
# Returns match from http headers
#######################################################

def matchWithHeaders():
	deviceFromHeaders = provider.getMatch(httpHeaders)
	outputProperties(deviceFromHeaders)

#########################################################
# Function which outputs match properties in table
#########################################################

def outputProperties(device):
	sys.stdout.write('   Id: %s\n' % device.getDeviceId())
	sys.stdout.write('   MatchMethod: %s\n' % device.getMethod())
	sys.stdout.write('   Difference: %s\n' % device.getDifference())
	sys.stdout.write('   Rank: %s\n' % device.getRank())
	for name in properties.split(','):
		value = device.getValues(name)
		if value:
			sys.stdout.write('   %s: %s\n' % (name, ' '.join(value)))
		else:
			sys.stdout.write('   %s: %s\n' % (name, 'N/A in Lite'))

def main():
########################################################
# Perform match with single User-Agent
########################################################
	title = 'Match with User-Agent'
	subtitle = 'User-Agent:\n   %s' % userAgent
	sys.stdout.write('\n%s\n%s\n\nDetected properties are...\n' % (title.center(50, '-'), subtitle))
	match()
########################################################
# Perform match with http headers
########################################################
	title = 'Match with HTTP headers'
	subtitle = 'HTTP headers:'
	sys.stdout.write('\n%s\n%s\n' % (title.center(50, '-'), subtitle))
	for header in httpHeaders.iterkeys():
		value = httpHeaders.__getitem__(str(header))
		sys.stdout.write('   %s: %s\n' % (header, value))
	sys.stdout.write('\nDetected properties are...\n')
	matchWithHeaders()


if __name__ == '__main__':
	main()
