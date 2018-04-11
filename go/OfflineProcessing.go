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
Offline processing example of using 51Degrees device detection. The example 
shows how to:
<ol>
<li>Instantiate the 51Degrees device detection provider.
<p><pre class="prettyprint lang-go">
var provider = FiftyOneDegreesPatternV3.NewProvider(dataFile)
</pre></p>
<li>Open an input file with a list of User-Agents, and an output file. 
<p><pre class="prettyprint lang-go">
fin, err := os.Open("20000 User Agents.csv")
fout, err := os.Create(outputFile)
</pre></p>
<li>Write a header to the output file with the property names in '|'
separated CSV format ('|' separated because some User-Agents contain
commas)
<p><pre class="prettyprint lang-go">
var out = [][]string{{"User-Agent"}}
for i := range properties {
	out[0] = append(out[0], properties[i])
}
</pre></p>
<li>For the User-Agents in the input file, perform a match then
write the User-Agent along with the values for chosen properties to
the csv.
<p><pre class="prettyprint lang-go">
for _,record := range records{
	// Perform a match on each record and append to an array.
	if len(record) > 0 {
		// Get the User-Agent from each record.
		ua := record

		// Used to store and access detection results.
		match := provider.GetMatch(ua)
		result := []string{ua}

		// Get value for each requested property and append to results.
		for i := range properties{
			result = append(result, match.GetValue(properties[i]))
		}
		out = append(out, result)
		// Close the match object.
		FiftyOneDegreesPatternV3.DeleteMatch(match)
	}
}
</pre></p>
</ol>
This example assumes you have the 51Degrees Go API installed correctly,
and have access to a source of User-Agents: "../data/20000 User Agents.csv"
and a binary data file: "../data/51Degrees-LiteV3.2.dat".
The output directory must also exist and have write permissions.
</tutorial>
*/

// Snippet Start
package main

import (
	"fmt"
	"./src/pattern"
	"encoding/csv"
	"os"
	"log"
	"io"
)

// Locations of data files.
const dataFile = "../data/51Degrees-LiteV3.2.dat"
const inputFile = "../data/20000 User Agents.csv"
const outputFile = "offlineProcessingOutput.csv"

// Number of records in the CSV file to process.
const numRecords = 20

// Provides access to device detection functions.
var provider =
	FiftyOneDegreesPatternV3.NewProvider(dataFile)

// Which properties to retrieve
var properties = []string{"IsMobile", "PlatformName", "PlatformVersion"}

func output_offline_processing() {

	// Set csv up reader
	fin, err := os.Open(inputFile)
	if err != nil{
		log.Fatal(err)
	}
	r := csv.NewReader(fin)
	// Separator between records in the input csv file is a pipe, this is 
	// because User-Agents can contain commas.
	r.Comma = '|'
	r.LazyQuotes = true

	// Set csv up writer
	fout, err := os.Create(outputFile)
	if err != nil {
		log.Fatal("Unable to create output file.")
	}
	w := csv.NewWriter(fout)
	// Set separator in output file to the pipe character, this is because
	// User-Agents can contain commas.
	w.Comma = '|'
	defer fout.Close()


	// Create a 2D array to append the match results to so we can write later.
	var out = [][]string{{"User-Agent"}}
	for i := range properties {
		out[0] = append(out[0], properties[i])
	}

	var records []string
	// Read a set number records from input file.
	if numRecords != -1 {
		for i := 0; i < numRecords; i++ {
			record, err := r.Read()
			// Stop at EOF.
			if err == io.EOF {
				break
			}
			// The first part of each record is the User-Agent, append it to
			// records array.
			records = append(records, record[0])
		}
	// Read all records from the input file.
	}else {
	    // Read all the records
	    record, _ := r.ReadAll()
	    // The first part of each record is the User-Agent, append it to
		// records array.
		for i := range record{
			records = append(records, record[i][0])
		}
	}

	// Process all records
	for _,record := range records{
		// Perform a match on each record and append to an array.
		if len(record) > 0 {
			// Get the User-Agent from each record.
			ua := record

			// Used to store and access detection results.
			match := provider.GetMatch(ua)
			result := []string{ua}

			// Get value for each requested property and append to results.
			for i := range properties{
				result = append(result, match.GetValue(properties[i]))
			}
			out = append(out, result)
			// Close the match object.
			FiftyOneDegreesPatternV3.DeleteMatch(match)

		}
	}
	fmt.Println("Done")

	// Write all processed records to the output file.
	w.WriteAll(out)

	defer w.Flush()
	fmt.Println()
}

func main() {
	fmt.Println("Starting Offline Prosessing")
	output_offline_processing()
	fmt.Println("Output file written to ", outputFile)
}
