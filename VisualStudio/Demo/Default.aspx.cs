/* *********************************************************************
 * This Source Code Form is copyright of 51Degrees Mobile Experts Limited.
 * Copyright 2015 51Degrees Mobile Experts Limited, 5 Charlotte Close,
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
 ********************************************************************** */

using System;
using System.Linq;
using FiftyOne.Mobile.Detection.Provider.Interop;
using System.Text;
using System.IO;

namespace FiftyOne.Demo.WebSite
{
    public partial class Default : System.Web.UI.Page
    {
        private const string DETECTION_PARAM_ROW = "<tr><th>{0}</th><td>{1}</td><td>{2}</td></tr>";

        // IMPORTANT: For a full list of properties see: 
        // https://51degrees.com/resources/property-dictionary

        protected void Page_Init(object sender, EventArgs e)
        {
            // Get properties for the current HTTP headers for each of the providers.
            // Use a "using" block to ensure the dispose method is called and any
            // unmanaged resources are freed before the method finishes. If this is 
            // not included then the dispose methods of the providers in the Global.asax
            // will enter an infinite wait.
            using (var patternMatch = Global.PatternProvider.Match(Request.Headers))
            {
                using (var trieMatch = Global.TrieProvider.Match(Request.Headers))
                {
                    // Output the properties from each provider.
                    var builder = new StringBuilder();
                    builder.Append("<p>For a full list of properties see <a href=\"https://51degrees.com/resources/property-dictionary\">property dictionary</a>.</p>");
                    builder.Append("<table>");
                    builder.Append("<tr><th></th><th>Pattern</th><th>Trie</th></tr>");

                    // Append common properties between the two providers.
                    foreach (var property in Global.PatternProvider.AvailableProperties.Where(i =>
                        i.Contains("Javascript") == false).Intersect(Global.TrieProvider.AvailableProperties))
                    {
                        builder.Append("<tr>");
                        builder.AppendFormat(
                            "<th>{0}</th>",
                            property);
                        builder.AppendFormat(
                            "<td>{0}</td>",
                            patternMatch[property]);
                        builder.AppendFormat(
                            "<td>{0}</td>",
                            trieMatch[property]);
                        builder.Append("</tr>");
                    }

                    // Append detection properties used to provide a confidence indicator
                    // concerning the matched results.
                    builder.AppendFormat(DETECTION_PARAM_ROW, "Matched User-Agent", patternMatch.UserAgent, trieMatch.UserAgent);
                    builder.AppendFormat(DETECTION_PARAM_ROW, "DeviceId", patternMatch.DeviceId, trieMatch.DeviceId);
                    builder.AppendFormat(DETECTION_PARAM_ROW, "Method", patternMatch.Method, trieMatch.Method);
                    builder.AppendFormat(DETECTION_PARAM_ROW, "Rank", patternMatch.Rank, trieMatch.Rank);
                    builder.AppendFormat(DETECTION_PARAM_ROW, "Difference", patternMatch.Difference, trieMatch.Difference);
                    builder.Append("</table>");
                    Results.Text = builder.ToString();
                }
            }
        }
    }
}