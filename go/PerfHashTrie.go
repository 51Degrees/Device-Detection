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
	"encoding/csv"
	"os"
	"log"
	"strings"
	"sync"
	"time"
)

type PERFORMANCE_STATE struct {
	userAgents [][]string
	userAgentsCount int
	count int
	progress int
	max int
	calibration int
	numberOfThreads int
}

// Set number of threads.
const threads  = 4

// Loop over all User-Agents read from file. If calibration is set to 1 
// (enabled) then do nothing.
func runPerformanceTest(provider FiftyOneDegreesTrieV3.Provider, 
                        state PERFORMANCE_STATE, 
                        records [][]string, 
                        properties string, 
                        wg *sync.WaitGroup) {
	defer wg.Done()
	var count = 0;

    // Loop over all User-Agents.
	for _, record := range records{
		if len(record[0]) < 1024 && state.calibration == 0 {
            // If calibrating, do nothing.
			match := provider.GetMatch(record[0])
			FiftyOneDegreesTrieV3.DeleteMatch(match);
		}
		count++
	}
}

// Perform the perfomance test. First calibrate by calculating the ammount of 
// time it takes to loop over all the User Agents and then remove this from the 
// final result.
func performanceTest(provider FiftyOneDegreesTrieV3.Provider, 
                     state PERFORMANCE_STATE, 
                     records [][]string, 
                     properties string) time.Duration {
	var wg sync.WaitGroup

	fmt.Println("Calibrating")
	state.calibration = 1
	startCal := time.Now()
	for i := 0; i < state.numberOfThreads; i++ {
		wg.Add(1)
		go runPerformanceTest(provider, state, records, properties, &wg)
	}
	wg.Wait()

	calibrateTime := time.Since(startCal)

	fmt.Println("Testing")
	state.calibration = 0
	startTes := time.Now()
	for i := 0; i < state.numberOfThreads; i++ {
		wg.Add(1)
		go runPerformanceTest(provider, state, records, properties, &wg)
	}
	wg.Wait()

	testTime := time.Since(startTes)

	totalTime := testTime - calibrateTime

	return totalTime

}

func perf_trie(provider FiftyOneDegreesTrieV3.Provider, inputFile string, properties string) {

	// Set csv up reader.
	fin, err := os.Open(inputFile)
	if err != nil{
		log.Fatal(err)
	}
	r := csv.NewReader(fin)
	// Separator between records in the input csv file is a new line.
	r.Comma = '|'
	r.LazyQuotes = true

	fmt.Println("Loading records...")
	records, err := r.ReadAll()

	if err != nil {
		log.Fatal(err)
	}
	state := PERFORMANCE_STATE{records, len(records), 0, 0, 25000, 0, threads}
	totalTime := performanceTest(provider, state, records, properties)

	fmt.Println("Time taken: ", totalTime)
	fmt.Printf("Detections per second: %.2f\n", ((float64(len(records)) * threads) / totalTime.Seconds()))
	fmt.Printf("Time per detection: %v\n", (totalTime.Seconds() * 1000 ) / (float64(len(records)) * threads))

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
	fmt.Print("\t#    Command line arguments should be a tree format data    #\n")
	fmt.Print("\t#   file and a csv file containing a list of user agents.   #\n")
	fmt.Print("\t#      A test file of 1 million can be downloaded from      #\n")
	fmt.Print("\t#            http://51degrees.com/million.zip               #\n")
	fmt.Print("\t#                                                           #\n")
	fmt.Print("\t#############################################################\n")

	filename := ""
	if len(args) > 1 {
		filename = args[0]
	} else {
		filename = "../data/51Degrees-LiteV3.4.trie"
	}

	inputFile := ""
	if len(args) > 2 {
		inputFile = args[1]
	} else {
		inputFile = "../data/20000 User Agents.csv"
	}

	requiredPropertiesArg := ""
	if len(args) > 3 {
		requiredPropertiesArg = args[2]
	} else {
		requiredPropertiesArg = ""
	}
	requiredProperties := ""

	if !strings.Contains("Id", requiredPropertiesArg) {
		fmt.Println(requiredProperties, "%s,Id", requiredPropertiesArg)
	} else
	{
		requiredProperties = requiredPropertiesArg
	}
	var provider =
		FiftyOneDegreesTrieV3.NewProvider(filename)

	// Run the performance tests.
	perf_trie(provider, inputFile, requiredProperties)
}
