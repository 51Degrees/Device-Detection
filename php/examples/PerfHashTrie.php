<?php
/*
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
*/

/*
<tutorial>
Hash Trie Performance example using 51Degrees device detection. The example
shows how to:
<ol>
<li>Fetch a pointer to the 51Degrees device detection provider instance.
This is instantiated on server startup and uses settings from php.ini.
<p><pre class="prettyprint lang-php">
$provider = FiftyOneDegreesTrieV3::provider_get();
</pre></p>
<li>Open an input file with a list of User-Agents.
<p><pre class="prettyprint lang-php">
$file_in = fopen("20000 User Agents.csv", "r");
</pre></p>
<li>Read user agents from a file and calculate the ammount of time it takes to 
match them all using the provider.
<p><pre class="prettyprint lang-php">
$time_start = microtime_float();
for ($z=0;$z<$records;$z++) {
    $userAgent = trim(fgets($file_in), "\n");
    $match = $provider->getMatch($userAgent);
}
$time_end = microtime_float();

$time = $time_end - $time_start;
</pre></p>
</ol>
<p>This example assumes you have the 51Degrees PHP API installed correctly.
See the instructions in the php readme file in this repository: 
(Device-Detection/php/README.md).</p>
<p>The examples also assumes you have access to a Hash Trie data file and
have set the path to "20000 User Agents.csv" correctly. Both of these files 
need to be available in the data folder in the root of this repository. Please 
see data/TRIE.txt for more details on downloading the Hash Trie data file.</p>
<p>A file containing 1-million User-Agents can be downloaded from 
https://51Degrees.com/million.zip </p>
</tutorial>
*/

// Snippet Start
// Function gets time in seconds with a degree of accuracy down to the
// microsecond.
function microtime_float()
{
    list($usec, $sec) = explode(" ",microtime());
    return ((float)$usec + (float)$sec);
}

// Get PHP C wrapper interface.
require("../trie/FiftyOneDegreesTrieV3.php");

// Reference to the 51Degrees provider.
$provider = FiftyOneDegreesTrieV3::provider_get();

// Input User-Agents file.
$inputFile = "../../data/20000 User Agents.csv";
// Number of records to process.
$records = 20000;
// Number of passes to do.
$passes = 5;

echo "Starting PerfTrie.<br><p>\n";

for ($x=0;$x<$passes;$x++){

    // Calibrate step: Opens input file, reads n(records) User-Agents from 
    // the input file.
    $file_in = fopen($inputFile, "r");

    // Read User-Agents file .
    $time_start = microtime_float();
    for ($y=0;$y<$records;$y++) {
	    $userAgent = trim(fgets($file_in), "\n");
        // Do nothing.
    }
    $time_end = microtime_float();
    // Get time to read User-Agents file.
    $calibrate =  $time_end - $time_start;
    fclose($file_in);

    // Benchmark performance step: Opens input file, reads n(records)
    // User-Agents from the input file performing a detection on each one.
    $file_in = fopen($inputFile, "r");
    
    // Read User-Agents file and perform a match for each User-Agent.
    $time_start = microtime_float();
    for ($z=0;$z<$records;$z++) {
	    $userAgent = trim(fgets($file_in), "\n");
	    $match = $provider->getMatch($userAgent);
    }
    $time_end = microtime_float();
    fclose($file_in);

    // Get time to read User-Agents file and perform detection for each
    // User-Agent.
    $time = $time_end - $time_start;


    // Calculate time for current pass and add to culmulative detection time
    // for all passes: 
    // Get total time for all detections minus the time taken to read the input
    // file.
    $detectionTime = $time - $calibrate;
    // Add detection time to detection time for all passes.
    $cumulativeTime = $cumulativeTime + $detectionTime;
    
    echo "Pass: ".($x+1)." - Detection Time: ".$detectionTime."<br>";
}

// Get average time for all passes.
$totalTime = $cumulativeTime / $passes;
// Calculate average detections per second.
$dps = $records / $totalTime;
// Calculate the time for a single detection.
$spd = ($dps**-1)*1000;

echo "</p>";
echo "Average time to process inputFile: ".$totalTime."<br>";
echo "Average Detections per second: ".$dps."<br>";
echo "Time for a single detection (ms):".$spd;

?>
