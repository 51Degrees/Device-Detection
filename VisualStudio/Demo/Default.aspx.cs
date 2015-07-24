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
        // Initialise the pattern provider to return all available properties.
        private static readonly PatternWrapper _pattern = new PatternWrapper(
            Path.Combine(
                AppDomain.CurrentDomain.BaseDirectory,
                "..\\..\\data\\51Degrees-LiteV3.2.dat"));

        // Initialise the trie provider to return all available properties.
        private static readonly TrieWrapper _trie = new TrieWrapper(
            Path.Combine(
                AppDomain.CurrentDomain.BaseDirectory, 
                "..\\..\\data\\51Degrees-LiteV3.2.trie"));

        // IMPORTANT: For a full list of properties see: 
        // https://51degrees.com/resources/property-dictionary

        protected void Page_Init(object sender, EventArgs e)
        {
            // Get properties for the current HTTP headers.
            var patternProperties = _pattern.Match(Request.Headers);
            var trieProperties = _trie.Match(Request.Headers);

            // Output the properties from each provider.
            var builder = new StringBuilder();
            builder.Append("<table>");
            builder.Append("<tr><th></th><th>Pattern</th><th>Trie</th></tr>");
            foreach (var property in _pattern.AvailableProperties.Where(i =>
                i.Contains("Javascript") == false).Intersect(_trie.AvailableProperties))
            {
                builder.Append("<tr>");
                builder.Append(String.Format(
                    "<td><b>{0}</b></td>",
                    property));
                builder.Append(String.Format(
                    "<td>{0}</td>",
                    patternProperties[property]));
                builder.Append(String.Format(
                    "<td>{0}</td>",
                    trieProperties[property]));
                builder.Append("</tr>");
            }
            builder.Append("</table>");
            Results.Text = builder.ToString();
        }
    }
}