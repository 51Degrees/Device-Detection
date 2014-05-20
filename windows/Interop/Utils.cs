/* *********************************************************************
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0.
 *
 * If a copy of the MPL was not distributed with this file, You can obtain
 * one at http://mozilla.org/MPL/2.0/.
 *
 * This Source Code Form is "Incompatible With Secondary Licenses", as
 * defined by the Mozilla Public License, v. 2.0.
 * ********************************************************************* */

using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;

namespace FiftyOne.Mobile.Detection.Provider.Interop
{
    internal static class Utils
    {
        /// <summary>
        /// Returns a list of key value pairs for the properties.
        /// </summary>
        /// <param name="userAgent">The csv results from a detection.</param>
        /// <param name="property">The index of the property key.</param>
        /// <param name="firstValue">The index of the first value.</param>
        /// <returns>The properties and device values for the device matching the useragent provided.</returns>
        internal static SortedList<string, List<string>> GetProperties(string csv, int property, int firstValue)
        {
            var properties = new SortedList<string, List<string>>();
            if (csv != null)
            {
                // Convert the char array to a list of key and value pairs.
                using (var reader = new StringReader(csv))
                {
                    var line = reader.ReadLine();
                    while (String.IsNullOrEmpty(line) == false)
                    {
                        var segments = line.Split(new[] { '|' }, StringSplitOptions.None);
                        properties.Add(
                                segments[property],
                                new List<string>(segments.Skip(firstValue)));
                        line = reader.ReadLine();
                    }
                }
            }
            return properties;
        }
    }
}