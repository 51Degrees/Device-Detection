#!/usr/bin/env python

from FiftyOneDegrees import fiftyone_degrees_mobile_detector_v3_wrapper

import sys
import time
import resource

liteDataFile = "../../data/51Degrees-LiteV3.2.dat"
premiumDataFile = "../../data/51Degrees-PremiumV3.2.dat"
enterpriseDataFile = "../../data/51Degrees-EnterpriseV3.2.dat"
inputFile = '../../data/20000 User Agents.csv'
properties = "IsMobile"

def speedtest(provider):
	fin = open(inputFile, 'r')
	speedtest_start = time.time()
	i = 0
	while i < 20000 :
		userAgent = fin.readline().rstrip('\n')
		match = provider.getMatch(userAgent)
		i += 1
	speedtest_end = time.time()
	fin.close()
	return (speedtest_end - speedtest_start)/(i/1000)

start = time.time()
provider = fiftyone_degrees_mobile_detector_v3_wrapper.Provider(
	liteDataFile,
	properties)
end = time.time()

print "Lite initialization time: %s ms" % ((end - start)*1000)
print "Lite time per detection: %s ms" % speedtest(provider)
print "Lite memory usage: %d Mb" % (resource.getrusage(resource.RUSAGE_SELF).ru_maxrss/1000)

del(provider)

start = time.time()
provider = fiftyone_degrees_mobile_detector_v3_wrapper.Provider(
	premiumDataFile,
	properties)
end = time.time()

print "Premium initialization time: %s ms" % ((end - start)*1000)
print "Premium time per detection: %s ms" % speedtest(provider)
print "Premium memory usage: %d Mb" % (resource.getrusage(resource.RUSAGE_SELF).ru_maxrss/1000)

del(provider)

start = time.time()
provider = fiftyone_degrees_mobile_detector_v3_wrapper.Provider(
	enterpriseDataFile,
	properties)
end = time.time()

print "Enterprise initialization time: %s ms" % ((end - start)*1000)
print "Enterprise time per detection: %s ms" % speedtest(provider)
print "Enterprise memory usage: %d Mb" % (resource.getrusage(resource.RUSAGE_SELF).ru_maxrss/1000)

del(provider)
