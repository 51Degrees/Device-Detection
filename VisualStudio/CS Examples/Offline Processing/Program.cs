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
Offline processing example of using 51Degrees device detection. 
The example shows how to:
<ol>
<li>Set the various settings for the 51Degrees detector
<p><code>
string fileName = args[0];<br>
string properties = "IsMobile,PlatformName,PlatformVersion";
</code></p>
<li>Instantiate the 51Degrees device detection provider
with these settings
<p><code>
Provider provider = new Provider(FileName, properties);
</code></p>
<li>Open an input file with a list of User-Agents, and an output file,
<p><code>
StreamReader fin = new StreamReader(inputFile);<br>
StreamWriter fout = new StreamWriter(outputFile);
</code></p>
<li>Write a header to the output file with the property names in '|'
separated CSV format ('|' sepparated because some User-Agents contain
commas)
<p><code>
fout.Write("User-Agent");<br>
for (i = 0; i < properties.Count(); i++ )<br>
{<br>
    fout.Write("|" + properties[i]);<br>
}<br>
fout.Write("\n");
</code></p>
<li>For the first 20 User-Agents in the input file, performa match then
write the User-Agent along with the values for chosen properties to
the CSV.
<p><code>
for (i = 1; i < 20; i++ )<br>
{<br>
    userAgent = fin.ReadLine();<br>
    match = provider.getMatch(userAgent);<br>
    fout.Write(userAgent);<br>
    for (j = 0; j < properties.Count(); j++ )<br>
    {<br>
        fout.Write("|" + match.getValue(properties[j]));<br>
    }<br>
    fout.Write("\n");<br>
}
</code></p>
</ol>
This tutorial assumes you are building this from within the
51Degrees Visual Studio solution. Running the executable produced
inside Visual Studio will ensure all the command line arguments
are preset correctly. If you are running outside of Visual Studio,
make sure to add the path to a 51Degrees data file and 
"20000 User Agents.csv" as arguments.
</tutorial>
*/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using FiftyOne.Mobile.Detection.Provider.Interop.Pattern;
using System.IO;

namespace FiftyOne.Example.Illustration.CSharp.OfflineProcessing
{
    public class Program
    {
        // Snippet Start
        public static void Run(string fileName, string inputFile)
        {
            int i, j;
            string outputFile = "OfflineProcessingOutput.csv";
            string userAgent;
            Match match;
            string propertiesList = "IsMobile,PlatformName,PlatformVersion";

            /*
             * Initialises the device detection dataset with the above settings.
             * This uses the Lite data file For more info
             * see:
             * <a href="https://51degrees.com/compare-data-options">compare data options
             * </a>
             */
            Provider provider = new Provider(fileName, propertiesList);

            // Fetched available properties as a VectorString.
            VectorString properties = provider.getAvailableProperties();

            // Opens input and output files.
            StreamReader fin = new StreamReader(inputFile);
            StreamWriter fout = new StreamWriter(outputFile);

            Console.WriteLine("Starting Offline Processing Example.");

            // Print CSV headers to output file.
            fout.Write("User-Agent");
            for (i = 0; i < properties.Count(); i++ )
            {
                fout.Write("|" + properties[i]);
            }
            fout.Write("\n");

            // Carries out match for first 20 User-Agents and prints results to
            // output file.
            for (i = 1; i < 20; i++ )
            {
                userAgent = fin.ReadLine();
                match = provider.getMatch(userAgent);
                fout.Write(userAgent);
                for (j = 0; j < properties.Count(); j++ )
                {
                    fout.Write("|" + match.getValue(properties[j]));
                }
                fout.Write("\n");
            }

            fin.Close();
            fout.Close();

            Console.WriteLine("Output Written to " + outputFile);
        }
        // Snippet End

        static void Main(string[] args)
        {
            Run(args[0], args[1]);

            // Waits for a character to be pressed.
            Console.ReadKey();
        }
    }
}
