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
using FiftyOne.Mobile.Detection.Provider.Interop;
using System.Text;
using System.IO;

namespace Demo
{
    public partial class Default : System.Web.UI.Page
    {
        // Initialise the pattern provider with a list of 4 properties.
        private static readonly PatternWrapper _pattern = new PatternWrapper(
            Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "..\\..\\data\\51Degrees-Lite.dat"),
                new[] { "Id", "IsMobile", "ScreenPixelsWidth", "ScreenPixelsHeight" });

        // Initialise the trie provider with a data file and a list of 4 properties.
        private static readonly TrieWrapper _trie = new TrieWrapper(
            Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "..\\..\\data\\51Degrees-Lite.trie"),
            new[] { "Id", "IsMobile", "ScreenPixelsWidth", "ScreenPixelsHeight" });

        // IMPORTANT: For a full list of properties see: http://51degrees.mobi/products/devicedata/propertydictionary

        protected void Page_Init(object sender, EventArgs e)
        {
            // Get properties for the current useragent.
            var patternProperties = _pattern.GetProperties(Request.UserAgent);
            var trieProperties = _trie.GetProperties(Request.UserAgent);

            // Output the properties from each provider.
            var builder = new StringBuilder();
            builder.Append("<table>");
            builder.Append("<tr><th></th><th>Pattern</th><th>Trie</th></tr>");
            foreach (var property in patternProperties)
            {
                builder.Append("<tr>");
                builder.Append(String.Format(
                    "<td><b>{0}</b></td>",
                    property.Key));
                builder.Append(String.Format(
                    "<td>{0}</td>",
                    String.Join(",", property.Value)));

                // Add the tree property if one exists.
                if (trieProperties.ContainsKey(property.Key))
                    builder.Append(String.Format(
                        "<td>{0}</td>",
                        String.Join(",", trieProperties[property.Key])));
                else
                    builder.Append("<td></td>");
                builder.Append("</tr>");
            }
            builder.Append("</table>");

            Results.Text = builder.ToString();
        }
    }
}