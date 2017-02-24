/* *********************************************************************
 * This Source Code Form is copyright of 51Degrees Mobile Experts Limited. 
 * Copyright 2017 51Degrees Mobile Experts Limited, 5 Charlotte Close,
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
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.IO;
using System.Text.RegularExpressions;

namespace FiftyOne.UnitTests.HttpHeaders
{
    public abstract class Combinations : Base
    {
        protected void OperaMini_Samsung()
        {
            base.Process(
                "(?i)Opera Mini",
                "(?i)SAMSUNG GT-I", 
                new Validation() {
                    { "BrowserName", "Opera" },
                    { "HardwareVendor", "Samsung" },
                });
        }

        protected void OperaMini_iPhone()
        {
            base.Process(
                "(?i)Opera Mini",
                @"^Mozilla/5\.0 \(iPhone; CPU iPhone OS ",
                new Validation() {
                    { "BrowserName", "Opera" },
                    { "HardwareVendor", "Apple" },
                });
        }

        protected void OperaMini_HTC()
        {
            base.Process(
                "(?i)Opera Mini",
                " HTC ",
                new Validation() {
                    { "BrowserName", "Opera" },
                    { "HardwareVendor", "HTC" },
                });
        }
    }
}
