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

using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace FiftyOne.UnitTests.Update.Premium
{
    [TestClass]
    public class PatternAll : PatternBase
    {
        protected override string DataFile
        {
            get { return Constants.PREMIUM_PATTERN_V32; }
        }

        [TestInitialize]
        public void Initialise()
        {
            if (_wrapper == null) { _wrapper = CreateWrapper(); }
        }

        [TestCleanup]
        public void CleanUp()
        {
            Dispose();
        }
        
        [TestMethod]
        [TestCategory("Premium"), TestCategory("Update"), TestCategory("Pattern"), TestCategory("All")]
        public void PremiumV32Pattern_Update_File_All()
        {
            UpdateTest(ReloadMode.File);
        }

        [TestMethod]
        [TestCategory("Premium"), TestCategory("Update"), TestCategory("Pattern"), TestCategory("All")]
        public void PremiumV32Pattern_Update_Memory_All()
        {
            UpdateTest(ReloadMode.Memory);
        }
    }
}
