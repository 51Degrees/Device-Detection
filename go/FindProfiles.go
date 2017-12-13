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
Find profiles example of using 51Degrees device detection. The example
shows how to:
<ol>
<li>Instantiate the 51Degrees device detection provider.
<p><pre class="prettyprint lang-go">
provider := FiftyOneDegreesPatternV3.NewProvider(dataFile)
</pre></p>
<li>Retrieve all the profiles in the data set with the specified property
value pair
<p><pre class="prettyprint lang-go">
profiles := provider.FindProfiles("IsMobile", "True")
</pre></p>
<li>Search within a list of profiles for another property value pair.
<p><pre class="prettyprint lang-go">
profiles = provider.FindProfiles("ScreenPixelsWidth", "1080", profiles)
</pre></p>
</ol>
This example assumes you have the 51Degrees Go API installed correctly.
</tutorial>
*/

// Snippet Start
package main

import (
    "fmt"
    "./src/pattern"
)

// Location of data file.
var dataFile = "../data/51Degrees-LiteV3.2.dat"

func main() {
    // Provides access to device detection functions.
    provider := FiftyOneDegreesPatternV3.NewProvider(dataFile)

    // Retrieve all mobile profiles from the data set.
    profiles := provider.FindProfiles("IsMobile", "True")
    fmt.Println("There are ", profiles.GetCount(), " mobile profiles in the ", provider.GetDataSetName(), " data file.")
    profiles = provider.FindProfiles("ScreenPixelsWidth", "1080", profiles)
    fmt.Println(profiles.GetCount(), " of them have a screen width of 1080 pixels.")

    // Retrieve all non-mobile profiles from the dataset.
    profiles = provider.FindProfiles("IsMobile", "False")
    fmt.Println("There are ", profiles.GetCount(), " non-mobile profiles in the ", provider.GetDataSetName(), " data file.")
    profiles = provider.FindProfiles("ScreenPixelsWidth", "1080", profiles)
    fmt.Println(profiles.GetCount(), " of them have a screen width of 1080 pixels.")
}
// Snippet End