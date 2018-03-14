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
Hash Trie Performance example using 51Degrees device detection. The example
shows how to:
<ol>
<li>Import settings from the 51Degrees settings file
<p><pre class="prettyprint lang-py">
data_file = settings.V3_TRIE_WRAPPER_DATABASE
properties = settings.PROPERTIES
</pre></p>
<li>Instantiate the 51Degrees device detection provider with these
properties
<p><pre class="prettyprint lang-py">
provider = fiftyone_degrees_mobile_detector_v3_trie_wrapper.Provider(data_file,
	properties)
</pre></p>
<li>Open an input file with a list of User-Agents.
<p><pre class="prettyprint lang-py">
file_in = open(input_file, 'r')
</pre></p>
<li>Read user agents from a file and calculate the ammount of time it takes to 
match them all using the provider.
<p><pre class="prettyprint lang-py">
start_time = datetime.datetime.now()
    for ua in range(records):
        user_agent = file_in.readline().rstrip('\n')
        match = provider.getMatch(user_agent)
    end_time = datetime.datetime.now()
    file_in.close()
</pre></p>
</ol>
<p>This example assumes you have the 51Degrees Python API installed correctly.
See the instructions in the Python readme file in this repository:
(Device-Detection/python/README.md).</p>
<p>The examples also assumes you have access to a Hash Trie data file and
have set the path to "20000 User Agents.csv" correctly. Both of these files 
need to be available in the data folder in the root of this repository. Please 
see data/TRIE.txt for more details on downloading the Hash Trie data file.</p>
<p>A file containing 1-million User-Agents can be downloaded from 
https://51Degrees.com/million.zip </p>
</tutorial>
'''
# // Snippet Start

from FiftyOneDegrees import fiftyone_degrees_mobile_detector_v3_trie_wrapper
from fiftyone_degrees.mobile_detector.conf import settings
import sys
import datetime

# Input data file
data_file = settings.V3_TRIE_WRAPPER_DATABASE
# Define properties
properties = settings.PROPERTIES
# Input User-Agents file.
input_file = '../../data/20000 User Agents.csv'
# Number of passes to do.
passes = 5

# Instantiate 51Degrees provider.
provider = fiftyone_degrees_mobile_detector_v3_trie_wrapper.Provider(
	data_file,
	properties)

print "Starting PerfHash.\n" 

# Variable defined as 0 in seconds with a degree of accuracy down to the microsecond.
# cumulative_time = datetime.timedelta(0, 0)

# Open the csv and count the number of User-Agents.
file_in = open(input_file, 'r')
lines = file_in.readlines()
file_in.close()

# define the number of User-Agents in the csv.
records = len(lines)
print ("There are %d records" % (records))

pass_count = 0

while pass_count < passes:
    # Calibration step: Opens input file, reads n(records) of User-Agents from 
    # the input file.
    file_in = open(input_file, 'r')
    # Read User-Agents file
    start_time = datetime.datetime.now()
    for ua in range(records):
        userAgent = file_in.readline().rstrip('\n')
        # Do nothing
    end_time = datetime.datetime.now()
    # Get time to read User-Agents file
    calibrate_time = end_time - start_time
    # Close the file
    file_in.close()

    # Benchmark step: Read the input file and perform a match for each User-Agent.
    file_in = open(input_file, 'r')

    start_time = datetime.datetime.now()

    for ua in range(records):
        user_agent = file_in.readline().rstrip('\n')
        match = provider.getMatch(user_agent)
    
    end_time = datetime.datetime.now()
    file_in.close()

    # Get time to read User-Agents file and perform detection for each
    # User-Agent.
    time_taken = end_time - start_time
    # Calculate time for current pass and add to culmulative detection time
    # for all passes: 
    # Get total time for all detections minus the time taken to read the input
    # file.
    detection_time = time_taken - calibrate_time
    # Add detection time to detection time for all passes.
    cumulative_time = datetime.timedelta(0,0)
    cumulative_time = cumulative_time + detection_time

    print("Pass: %d of %d - Detection Time: %f" % (pass_count + 1,passes, detection_time.total_seconds()))

    pass_count += 1

# Get average time for all passes.
total_time = cumulative_time.total_seconds() / passes
# Calculate average detections per second.
dps = records / total_time
# Calculate the time for a single detection.
time_per_detection = dps**-1

# Print the results
print ("Average time to process inputFile: %f" % (total_time))
print ("Average Detections per second: %f" % (dps))
print ("Time for a single detection: %f" % (time_per_detection * 1000))



    
