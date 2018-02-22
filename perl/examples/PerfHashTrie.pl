#!/usr/bin/env perl
=copyright
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
=cut

=tutorial
<tutorial>

Hash Trie Performance example using 51Degrees device detection. The example
shows how to:
<ol>
<li>Set the various settings for 51Degrees detector
<p><pre class="prettyprint lang-pl">
my $filename = "51Degrees-LiteV3.4.trie";
my $propertyList = "IsMobile"
</pre></p>
<li>Instantiate the 51Degrees device detection provider with these
properties
<p><pre class="prettyprint lang-pl">
my $provider = new FiftyOneDegrees::TrieV3::Provider(
	$dataFile,
	$properties);
</pre></p>
<li>Open an input file with a list of User-Agents
<p><pre class="prettyprint lang-pl">
open my $file_in, "../../data/20000 User Agents.csv";
</pre></p>
<li>Read user agents from a file and calculate the ammount of time it takes to 
match them all using the provider.
<p><pre class="prettyprint lang-pl">
$start = gettimeofday();
while($i<=$#lines) {
    # Perform a match for the current User-Agent.
    	my $match = $provider->getMatch($lines[$i]);
    $i++;
}
$end = gettimeofday();

my $detectionTime = $end - $start;
</pre></p>
</ol>
<p>This example assumes you have the 51Degrees Perl API installed correctly,
see the instructions in the Perl readme file in this repository:
(Device-Detection/perl/README.md).</p>
<p>The examples also assumes you have access to a Hash Trie data file and
have set the path to "20000 User Agents.csv" correctly. Both of these files 
need to be available in the data folder in the root of this repository. Please 
see data/TRIE.txt for more details on downloading the Hash Trie data file.</p>
<p>A file containing 1-million User-Agents can be downloaded from 
https://51Degrees.com/million.zip </p>
</tutorial>
=cut
# // Snippet Start
use strict;
use warnings;
use threads;
use threads::shared;
use FiftyOneDegrees::TrieV3;
use feature qw/say/;
use Time::HiRes qw/gettimeofday/;

use constant THREADS    => 4;
use constant PASSES     => 5; 

my $filename = "../../data/51Degrees-LiteV3.4.trie";
my $propertyList = "IsMobile";
my $inputFile = "../../data/20000 User Agents.csv";

# Initialises the device detection provider with the settings declared above.

my $provider = new FiftyOneDegrees::TrieV3::Provider(
	$filename,
	$propertyList);

# Fetches an array of available properties from the provider.
my $properties = $provider->getAvailableProperties();

open my $file_in, '<', $inputFile;
chomp(my @lines = <$file_in>);
close $file_in;
my $count = scalar @lines;

say "Number of records: $count";

# Perform the perfomance test. First calibrate by calculating the ammount of 
# time it takes to loop over all the User Agents and then remove this from the 
# final result.
sub performTest{
    
    # Calibrate.
    my $i=0;
    my $start = gettimeofday();
    while($i<=$#lines) {
        # Do nothing.
        $i++;
    }
    my $end = gettimeofday();
    my $calibrateTime = $end - $start;

    # Perform Test.
    $i=0;
    $start = gettimeofday();
    while($i<=$#lines) {
        # Perform a match for the current User-Agent.
	    my $match = $provider->getMatch($lines[$i]);
        $i++;
    }
    $end = gettimeofday();

    my $detectionTime = $end - $start;

    # Calculate time
    my $time = $detectionTime - $calibrateTime;

    return $time;
}

say "Starting PerfHashTrie Perl.";

# Perform test for given number of threads and passes.
my $cumulative_time = 0;
my $num_threads = shift || THREADS;
for (my $i=0; $i<PASSES; $i++){
    my @threads = map { threads->create(\&performTest); } 1 .. $num_threads;
    my $thread_time = 0;
    my $average_thread_time = 0;
    for my $t (@threads) {
        $thread_time += $t->join();
    }
    $average_thread_time = $thread_time / $num_threads;
    $cumulative_time += $average_thread_time ;
    say "Pass: $i - Dection time: $average_thread_time"; 
}

# Calculate the average time it takes to match all the User-Agents
my $average_time = $cumulative_time / PASSES / $num_threads;
# Calculate the number of detections per second.
my $dps = $count / $average_time;

say "Average time to detect dataset: $average_time";
say "Average detections per second: $dps";
# Calculate the time for a single detection.
printf("Time for a single detection (ms): %.4f\n",($dps**-1)*1000); 


