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
Match for device id example of using 51Degrees device detection. 
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
<li>Produce a match for a single device id
<p><code>
match = provider.getMatchForDeviceId(deviceId);
</code></p>
<li>Extract the value of the IsMobile property
<p><code>
IsMobile = match.getValue("IsMobile");
</code></p>
</ol>
This tutorial assumes you are building this from within the
51Degrees Visual Studio solution. Running the executable produced
inside Visual Studio will ensure all the command line arguments
are preset correctly. If you are running outside of Visual Studio,
make sure to add the path to a 51Degrees data file as an argument.
</tutorial>
*/
// Snippet Start
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using FiftyOne.Mobile.Detection.Provider.Interop.Pattern;

namespace Match_For_Device_Id
{
    public class Program
    {
        static void run(string fileName)
        {
            string IsMobile;
            Match match;

            // Device id string of an iPhone mobile device.
            string mobileDeviceId = "12280-48866-24305-18092";

            // Device id string of Firefox Web browser version 41 on dektop.
            string desktopDeviceId = "15364-21460-53251-18092";

            // Device id string of a MediaHub device.
            string mediaHubDeviceId = "41231-46303-24154-18092";

            /**
            * Initialises the device detection dataset with the above settings.
            * This uses the Lite data file For more info
            * see:
            * <a href="https://51degrees.com/compare-data-options">compare data options
            * </a>
            */
            Provider provider = new Provider(fileName);

            Console.WriteLine("Starting Match For Device Id Example.");

            // Carries out a match for a mobile device id.
            match = provider.getMatchForDeviceId(mobileDeviceId);
            Console.WriteLine("\nMobile Device Id: " + mobileDeviceId);
            IsMobile = match.getValue("IsMobile");
            Console.WriteLine("   IsMobile: " + IsMobile);

            // Carries out a match for a desktop device id.
            match = provider.getMatchForDeviceId(desktopDeviceId);
            Console.WriteLine("\nDesktop Device Id: " + desktopDeviceId);
            IsMobile = match.getValue("IsMobile");
            Console.WriteLine("   IsMobile: " + IsMobile);

            // Carries out a match for a MediaHub device id.
            match = provider.getMatchForDeviceId(mediaHubDeviceId);
            Console.WriteLine("\nMediaHub Device Id: " + mediaHubDeviceId);
            IsMobile = match.getValue("IsMobile");
            Console.WriteLine("   IsMobile: " + IsMobile);
        }
        static void Main(string[] args)
        {
            run(args[0]);

            // Waits for a character to be pressed.
            Console.ReadKey();
        }
    }
}
// Snippet End