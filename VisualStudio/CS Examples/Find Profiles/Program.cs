/**
 * This Source Code Form is copyright of 51Degrees Mobile Experts Limited.
 * Copyright (c) 2015 51Degrees Mobile Experts Limited, 5 Charlotte Close,
 * Caversham, Reading, Berkshire, United Kingdom RG4 7BY
 * 
 * This Source Code Form is the subject of the following patent
 * applications, owned by 51Degrees Mobile Experts Limited of 5 Charlotte
 * Close, Caversham, Reading, Berkshire, United Kingdom RG4 7BY:
 * European Patent Application No. 13192291.6; and
 * United States Patent Application Nos. 14/085,223 and 14/085,301.
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0.
 * 
 * If a copy of the MPL was not distributed with this file, You can obtain
 * one at http://mozilla.org/MPL/2.0/.
 * 
 * This Source Code Form is "Incompatible With Secondary Licenses", as
 * defined by the Mozilla Public License, v. 2.0.
 */

/*
<tutorial>
Find profiles example of using 51Degrees device detection. 
The example shows how to:
<ol>
<li>Set the various settings for the 51Degrees detector
<p><code>
string properties = "IsMobile";<br>
string fileName = args[0];
</code></p>
<li>Instantiate the 51Degrees device detection provider
with these settings
<p><code>
Provider provider = new Provider(FileName, properties);
</code></p>
<li>Retrive all the profiles from the data set which match
a specified property value pair
<p><code>
Profiles profiles = provider.findProfiles("IsMobile", "True");
</code></p>
</ol>
This tutorial assumes you are building this from within the
51Degrees Visual Studio solution. Additionaly, when running,
the location of a 51Degrees data file must be passed as a 
command line argument if you wish to use premium or enterprise
</tutorial>
*/
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using FiftyOne.Mobile.Detection.Provider.Interop.Pattern;

namespace FiftyOne.Example.Illustration.CSharp.FindProfiles
{
    public class Program
    {
        // Snippet Start
        public static void Run(string fileName)
        {
            string properties = "IsMobile";

            /**
            * Initialises the device detection dataset with the above settings.
            * This uses the Lite data file For more info
            * see:
            * <a href="https://51degrees.com/compare-data-options">compare data options
            * </a>
            */
            Provider provider = new Provider(fileName, properties);

            Console.WriteLine("Starting Find Profiles Example.\n");
            
            Profiles profiles = provider.findProfiles("IsMobile", "True");
            Console.WriteLine("There are '{0}' mobile profiles in the '{1}' data set.",
                profiles.getCount(),
                provider.getDataSetName());

            Assert.IsNotNull(profiles);
            Assert.IsTrue(profiles.getCount() != 0);

            profiles = provider.findProfiles("IsMobile", "False");
            Console.WriteLine("There are '{0}' non-mobile profiles in the '{1}' data set.",
                profiles.getCount(),
                provider.getDataSetName());

            Assert.IsNotNull(profiles);
            Assert.IsTrue(profiles.getCount() != 0);
        }
        // Snippet End

        static void Main(string[] args)
        {
            string fileName = args.Length > 0 ? args[0] : "../../../../../../data/51Degrees-LiteV3.2.dat";
            Run(fileName);

            // Wait for a character to be pressed.
            Console.ReadKey();
        }
    }
}
