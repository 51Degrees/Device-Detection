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

// Snippet Start
package main

import (
    "fmt"
    "net/http"
    "./src/pattern"
    "html/template"
    "os"
)

// Location of data file.
var dataFile = "../data/51Degrees-LiteV3.2.dat"

// Global Provider object.
var provider FiftyOneDegreesPatternV3.Provider
var properties []string

// Type definitions which represent the data  used to populate the web page
// templates.

// Object is populated when a match is performed after a request. Used to
// inject values into HTML template before serving to requesting device.
type exampleTemplate struct{
    DatasetPublishedDate string
    DatasetNextUpdate string
    DatasetVersion string
    DatasetName string
    DatasetDeviceCombinations int
    DeviceId string
    MatchMethod int
    Difference int
    Rank int
    DeviceData template.HTML
}

// Object is populated with  a dynamically generated html table containing a
// list of properties and their values retrieved from the match object.
type usingDetectionTemplate struct{
	DeviceData template.HTML
}

// Function sets up list of available properties in the data set, translating
// from SWIG Vector string to go array.
func getAvailableProperties() []string{
	// Get available properties from the data set.
	var _properties = provider.GetAvailableProperties()
	// Get the length of the VectorString and convert to an iterable type.
	var len = int(_properties.Size())
	// Variable to hold list of properties in a native type.
	var nativeProperties []string

	// Loop over VectorString of available properties and append them to  native
	// type array.
	for i := 0; i < len; i++{
		nativeProperties = append(nativeProperties, _properties.Get(i))
	}

	return nativeProperties
}

// Http response handlers.
// Index
func handler(w http.ResponseWriter, r *http.Request) {
    templ, err := template.ParseFiles("html/index.html")
    err = templ.Execute(w, nil)
    checkError(err)
}

// Using Detection
func usingDetection(w http.ResponseWriter, r *http.Request) {
	// Get using-detection page HTML tmeplate.
    templ, err := template.ParseFiles("html/using-detection.html")
	// Get a match with the request User-Agent.
	match := provider.GetMatch(r.Header.Get("User-Agent"))
	// Variable to hold dynamically generated list of properties and their
	// values.
	var deviceData string

	// Dynamically build an HTML list of all the available properties in the
	// data set and their values retrieved from the match object.
	for _,property := range properties{
		values := match.GetValue(property)
		if values != "" {
			deviceData += "<li>" + property + ": " + values + "</li>"
		}else{
			deviceData += "<li>" + property + ": " + "Switch Data Set" + "</li>"
		}
	}
	// Populate usingDetectionTemplate struct
	d := &usingDetectionTemplate{
		DeviceData: template.HTML(deviceData)}

	// Execute template processor which injects values from type
	// usingDetectionTemplate and sends response to requesting device.
    err = templ.Execute(w, d)
    checkError(err)

	// Close the match object.
	FiftyOneDegreesPatternV3.DeleteMatch(match)
}

// Example page
func example(w http.ResponseWriter, r *http.Request) {
	// Define which properties are displayed on the example page.
    _properties := []string{"BrowserName", "BrowserVendor",
        "BrowserVersion", "DeviceType", "HardwareVendor", "IsTablet", 
        "IsMobile", "IsCrawler", "ScreenInchesDiagonal","ScreenPixelsWidth"}

	// Get example page HTML template.
    templ, err := template.ParseFiles("html/example.html")
    // Get a Match with the request User-Agent.
    match := provider.GetMatch(r.Header.Get("User-Agent"))
    // Variable to hold dynamically generated table.
    var deviceData string

    // Build property table dynamically to inject into template. Get values for
    // each property and append a row to the HTML table.
    for _,property := range _properties{
        values := match.GetValue(property)
        if values != "" {
            deviceData += "<tr><td><a href=\"https://51degrees.com/resources/" +
				"property-dictionary#"+ property + "\">" + property + "</a>" +
				"</td><td>" + values + "</td></tr>"
        }else{
			deviceData += "<tr><td><a href=\"https://51degrees.com/resources/" +
				"property-dictionary#"+ property + "\">" + property + "</a>" +
				"</td><td>Switch Dataset</td></tr>"
        }
    }

    // Populate exampleTemplate struct
    d := &exampleTemplate{
    DatasetPublishedDate: provider.GetDataSetPublishedDate(),
    DatasetNextUpdate: provider.GetDataSetNextUpdateDate(),
    DatasetVersion: provider.GetDataSetFormat(),
    DatasetName: provider.GetDataSetName(),
    DatasetDeviceCombinations: provider.GetDataSetDeviceCombinations(),
    DeviceId: match.GetDeviceId(),
    MatchMethod: match.GetMethod(),
    Difference: match.GetDifference(),
    Rank: match.GetRank(),
    DeviceData: template.HTML(deviceData)}

    // Execute template processor which injects values from type exampleTemplate
	// and sends response to requesting device.
    err = templ.Execute(w, d)
    checkError(err)

    // Release the Match object back to the pool.
	FiftyOneDegreesPatternV3.DeleteMatch(match)
}

func main() {
	fmt.Println("Starting Server...")

    // Create a new Provider.
    provider =
		FiftyOneDegreesPatternV3.NewProvider(dataFile)

	// Get available properties and assign to to global type.
	properties = getAvailableProperties()

    // Set the HTTP handler function.
    http.HandleFunc("/", handler)
    http.HandleFunc("/example", example)
    http.HandleFunc("/using-detection", usingDetection)

    // Start the Server and bind to port 8080.
    fmt.Println("Server at port 8080")
    err := http.ListenAndServe(":8080", nil)
    checkError(err)
}

func checkError(err error){
    if err != nil {
        fmt.Println("Fatal error", err.Error())
        os.Exit(1)
    }
}
// Snippet End