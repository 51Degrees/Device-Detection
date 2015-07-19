/* *********************************************************************
 * This Source Code Form is copyright of 51Degrees Mobile Experts Limited. 
 * Copyright © 2014 51Degrees Mobile Experts Limited, 5 Charlotte Close,
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
 * This Source Code Form is “Incompatible With Secondary Licenses”, as
 * defined by the Mozilla Public License, v. 2.0.
 * ********************************************************************* */

using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;
using System.Text;

namespace FiftyOne.Mobile.Detection.Provider.Interop
{
    internal static class Utils
    {
        /// <summary>
        /// Iterates through the CSV file returning the different
        /// segments.
        /// </summary>
        /// <param name="csv"></param>
        /// <returns></returns>
        private static IEnumerator<string> GetCsvIterator(StringBuilder csv)
        {
            int pos = 0, last = 0;
            while (pos < csv.Length)
            {
                if ((csv[pos] == ',' ||
                    csv[pos] == '\r' ||
                    csv[pos] == '\n') &&
                    pos != last)
                {
                    yield return csv.ToString(last, pos - last);
                    last = pos + 1;
                }
                pos++;
            }
        }

        /// <summary>
        /// Returns a list of key value pairs for the properties.
        /// </summary>
        /// <param name="userAgent">The csv results from a detection.</param>
        /// <param name="property">The index of the property key.</param>
        /// <param name="firstValue">The index of the first value.</param>
        /// <returns>The properties and device values for the device matching the useragent provided.</returns>
        internal static SortedList<string, List<string>> GetProperties(StringBuilder csv, int property, int firstValue)
        {
            var properties = new SortedList<string, List<string>>();
            if (csv != null)
            {
                string fieldName = null;
                var iterator = GetCsvIterator(csv);
                while (iterator.MoveNext())
                {
                    if (fieldName == null)
                    {
                        fieldName = iterator.Current;
                    }
                    else
                    {
                        try
                        {
                            properties.Add(fieldName, iterator.Current.Split('|').ToList());
                        }               
                        catch(Exception)
                        {
                            // Do nothing. TODO change this.
                        }
                        fieldName = null;
                    }
                }
            }
            return properties;
        }
    }
}