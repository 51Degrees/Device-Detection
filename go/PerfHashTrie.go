/* *********************************************************************
 * This Source Code Form is copyright of 51Degrees Mobile Experts Limited.
 * Copyright 2017 51Degrees Mobile Experts Limited, 5 Charlotte Close,
 * Caversham, Reading, Berkshire, United Kingdom RG4 7BY
 *
 * This Source Code Form is the subject of the following patents and patent
 * applications, owned by 51Degrees Mobile Experts Limited of 5 Charlotte
 * Close, Caversham, Reading, Berkshire, United Kingdom RG4 7BY:
 * European Patent No. 2871816;
 * European Patent Application No. 17184134.9;
 * United States Patent Nos. 9,332,086 and 9,350,823; and
 * United States Patent Application No. 15/686,066.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0.
 *
 * If a copy of the MPL was not distributed with this file, You can obtain
 * one at http://mozilla.org/MPL/2.0/.
 *
 * This Source Code Form is "Incompatible With Secondary Licenses", as
 * defined by the Mozilla Public License, v. 2.0.
 ********************************************************************** */

/*
<tutorial>
Performance benchmark example of using 51Degrees Hash Trie device detection. 
The example shows how to:
<ol>
<li>Instantiate the 51Degrees device detection provider.
<p><pre class="prettyprint lang-go">
var provider = FiftyOneDegreesTrieV3.NewProvider(dataFile)
</pre></p>
<li>Open an input file with a list of User-Agents.
<p><pre class="prettyprint lang-go">
fin, err := os.Open("20000 User Agents.csv")
</pre></p>
<li>Read user agents from a file and calculate the ammount of time it takes to 
match them all using the provider.
</ol>
This example assumes you have the 51Degrees Go API installed correctly,
see the instructions in the Go readme file in this repository:
(Device-Detection/go/README.md)
<p>The examples also assumes you have access to a Hash Trie data file and
have set the path to "20000 User Agents.csv" correctly. Both of these files 
need to be available in the data folder in the root of this repository. Please 
see data/TRIE.txt for more details on downloading the Hash Trie data file.</p>
</tutorial>
*/

// Snippet Start
package main

import (
	"fmt"
	"./src/trie"
	"os"
	"log"
	"strings"
	"sync"
	"time"
	"io/ioutil"
)

// Used to control multi threaded performance.
type performanceState struct {
	userAgents []string
	userAgentsCount int
	count int
	max int
	progress int
	calibration int
	numberOfThreads int
}

// Set number of threads.
const threads = 4
// Set number of passes.
const passes = 5
// Set progress marks.
const progressMarks = 40

// Lock object used to synchronize threads when reporting progress.
var mutex = &sync.Mutex{}

// Report progress of test.
func reportProgress(state *performanceState, count int, device string){
	// Lock the state whilst the counters are updated
	mutex.Lock()

	// Increase the count.
	state.count += count
	full := state.count / state.progress
	empty := (state.max - state.count) / state.progress

	// Update the UI.
	fmt.Printf("\r\t[")
	for i := 0; i < full; i++ {
		fmt.Print("=")
	}
	for j := 0; j < empty; j++ {
		fmt.Print(" ")
	}
	fmt.Print("]")

	// If in real detection mode then print the id of the device found to prove
	// the test is actually doing something!
	if state.calibration == 0 {
		fmt.Printf(" %s ", device)
	}

	// Unlock the state now that the count has been updated.
	mutex.Unlock()
}

// Loop over all User-Agents read from file. If calibration is set to 1 
// (enabled) then do nothing.
func runPerformanceTest(provider FiftyOneDegreesTrieV3.Provider,
                        state *performanceState,
                        wg *sync.WaitGroup) {
	defer wg.Done()
	var count = 0
	var device string

    // Loop over all User-Agents.
	for _, record := range state.userAgents{
		// If we're not calibrating then get the device for the User-Agent
		// that had just been read.
		if len(record) < 1024 && state.calibration == 0 {
            // If calibrating, do nothing.
			match := provider.GetMatch(record)
			device = match.GetDeviceId()
			FiftyOneDegreesTrieV3.DeleteMatch(match);
		}
		// Increase the local counter.
		count++

		// Print a progress marker.
		if count == state.progress {
			reportProgress(state, count, device);

			// Reset the local counter.
			count = 0
		}
	}
	// Finally report progress.
	reportProgress(state, count, device);
}

// Execute a performance test using a file of null terminated useragent strings
// as input. If calibrate is true then the file is read but no detections
// are performed.
func performanceTest(provider FiftyOneDegreesTrieV3.Provider,
                     state performanceState)  {
	var wg sync.WaitGroup

	// Create the threads.
	for i := 0; i < state.numberOfThreads; i++ {
		wg.Add(1)
		go runPerformanceTest(provider, &state, &wg)
	}

	// Wait for threads to finish.
	wg.Wait()
}

// Perform the test and return the average time.
func performTest(provider FiftyOneDegreesTrieV3.Provider,
					state performanceState, test string) time.Duration {
	start := time.Now()

	// Perform the test for a number of passes.
	for pass := 1; pass <= passes; pass++ {
		fmt.Printf("\r\n%s pass %d of %d: \n\n", test, pass, passes)
		performanceTest(provider, state)
	}
	end := time.Since(start)

	// Return the average time taken to complete the test.
	return end / passes;
}

// Performance test.
func perf_trie(provider FiftyOneDegreesTrieV3.Provider, inputFile string) {
	// Read the User-Agents into an array.
	content, err := ioutil.ReadFile(inputFile)
	if err != nil {
		log.Fatal(err)
	}
	records := strings.Split(string(content), "\n")

	// Get the number of records, used to print progress bar.
	numrecords := len(records)
	max := numrecords * threads
	progress := max / progressMarks

	state := performanceState{records, numrecords, 0, max, progress,1, threads}

	// Run the process without doing any detections to get a calibration time.
	calbration := performTest(provider, state, "Calibration")

	// Process the User-Agents doing device detection.
	state.calibration = 0;
	test := performTest(provider, state, "Detection test")

	totalTime := test - calbration

	fmt.Println("\r\n")
	fmt.Println("Time taken for a single thread: ", totalTime)
	fmt.Printf("Detections per second for %d thread(s): %.2f\n",
		threads,
		((float64(len(records)) * threads) / totalTime.Seconds()))
	fmt.Printf("Time per detection (ms): %v\n",
		(totalTime.Seconds() * 1000 ) / (float64(len(records)) * threads))

}

func main() {
	args := os.Args[1:]
	
	fmt.Print("\n")
	fmt.Print("\t#############################################################\n")
	fmt.Print("\t#                                                           #\n")
	fmt.Print("\t#  This program can be used to test the performance of the  #\n")
	fmt.Print("\t#              51Degrees 'Hash Trie' Go API.                #\n")
	fmt.Print("\t#                                                           #\n")
	fmt.Print("\t#   The test will read a list of User Agents and calculate  #\n")
	fmt.Print("\t#            the number of detections per second.           #\n")
	fmt.Print("\t#                                                           #\n")
	fmt.Print("\t#    Command line arguments should be a trie format data    #\n")
	fmt.Print("\t#   file and a csv file containing a list of user agents.   #\n")
	fmt.Print("\t#      A test file of 1 million can be downloaded from      #\n")
	fmt.Print("\t#            http://51degrees.com/million.zip               #\n")
	fmt.Print("\t#                                                           #\n")
	fmt.Print("\t#############################################################\n")
	fmt.Print("\n")

	filename := ""
	if len(args) > 0 {
		filename = args[0]
	} else {
		filename = "../data/51Degrees-LiteV3.4.trie"
	}

	userAgentsFile := ""
	if len(args) > 1 {
		userAgentsFile = args[1]
	} else {
		userAgentsFile = "../data/20000 User Agents.csv"
	}

	requiredPropertiesArg := ""
	if len(args) > 2 {
		requiredPropertiesArg = args[2]
	} else {
		requiredPropertiesArg = ""
	}

	fmt.Println("Data file: ", filename)
	fmt.Println("User-Agents file: ", userAgentsFile)

	var provider =
		FiftyOneDegreesTrieV3.NewProvider(filename, requiredPropertiesArg)

	// Run the performance tests.
	perf_trie(provider, userAgentsFile)
}
