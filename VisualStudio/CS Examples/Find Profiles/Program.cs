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
Find profiles example of using 51Degrees device detection. The example shows 
how to:
<ol>
    <li>Initialise detector with path to the 51Degrees device data file and 
    a list of properties.
    <p><code>
        string properties = args[1];<br />
        string fileName = args[0];<br />
        Provider provider = new Provider(FileName, properties);
    </code></p>
    <li>Use Provider to retrieve a list of profiles that match some given 
    property : value pair.
    <p><code>
        Profiles profiles;<br />
        using (profiles = provider.findProfiles(property, value))<br />
        {<br />
            // Do something with the list.<br />
        }<br />
    </code></p>
    <li> Dispose of the Provider releasing the resources.
    <p><code>
        provider.Dispose();
    </code></p>
</ol>
<p>
    This tutorial assumes you are building this example using Visual Studio. 
    You should supply path to the 51Degrees device data file that you wish to 
    use as a command line argument.
</p>
<p>
    By default the API is distributed with Lite data which is free to use for 
    both the non-commercial and commercial purposes. Lite data file contains 
    over 60 properties. For more properties like DeviceType, PriceBand and 
    HardwareVendor check out the Premium and Enterprise data files:
    https://51degrees.com/compare-data-options
</p>
<p>
    Passing a list of properties to the provider constructor limits the number 
    of properties in the dataset the provider wraps to only the chosen 
    properties. Not providing a list or providing an empty list will cause the 
    dataset to be created with all available properties:
    <br /><code>
        Provider provider = new Provider(fileName, "");
        Provider provider = new Provider(fileName);
    </code>
</p>
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
        /// <summary>
        /// Used for testing. Runs the program with the path provided and 
        /// only the IsMobile property.
        /// </summary>
        /// <param name="fileName">
        /// Path to the 51Degrees device data file.
        /// </param>
        public static void Run(string fileName)
        {
            Run(fileName, "IsMobile");
        }

        // Snippet Start

        /// <summary>
        /// Runs the program with the provided path to the data file and list 
        /// of comma-separated properties.
        /// </summary>
        /// <param name="fileName">
        /// Path to the 51Degrees device data file.
        /// </param>
        /// <param name="properties">
        /// Comma-separated list of properties to initialise the data set with.
        /// </param>
        public static void Run(string fileName, string properties)
        {
            // Use path to the data file and a list of properties to create 
            // provider.
            Provider provider = new Provider(fileName, properties);
            Console.WriteLine(provider.getAvailableProperties().Count);
            Console.WriteLine("Starting Find Profiles Example.\n");

            // Find all profiles for devices classified as mobile.
            findProfiles(provider, "IsMobile", "True");

            // Find all profiles for devices classified as non-mobile.
            findProfiles(provider, "IsMobile", "False");

            // At the end of the program dispose of the data file to 
            // deallocate memory.
            provider.Dispose();
        }

        /// <summary>
        /// Generates a list of profiles where the required property is equal 
        /// to the required value. List may be empty if either the selected 
        /// property is not present in the data file, or if the value for the 
        /// chosen property was not found. 
        /// </summary>
        /// <param name="provider">
        /// FiftyOne Provider that provides methods to interact with the 
        /// 51Degrees device data file.
        /// </param>
        /// <param name="property">
        /// Name of the property that the profile should contain.
        /// </param>
        /// <param name="value">
        /// Value for the above property to search for.
        /// </param>
        public static void findProfiles(Provider provider, 
                                        string property, 
                                        string value)
        {
            Profiles profiles;

            using (profiles = provider.findProfiles(property, value)) 
            {
                Console.WriteLine("There are '{0}' '{1}'" +
                "profiles in the '{2}' data set.",
                profiles.getCount(),
                value,
                provider.getDataSetName());
                Assert.IsNotNull(profiles);
                Assert.IsTrue(profiles.getCount() != 0);
            }
        }
        // Snippet End

        static void Main(string[] args)
        {
            string fileName = args.Length > 0 ? args[0] : 
                "../../../../../../data/51Degrees-LiteV3.2.dat";
            string properties = args.Length > 1 ? args[1] : "IsMobile";
            Run(fileName, properties);

            // Wait for a character to be pressed.
            Console.ReadKey();
        }
    }
}
