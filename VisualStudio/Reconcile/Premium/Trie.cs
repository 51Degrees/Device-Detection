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
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace FiftyOne.Reconcile.Premium
{
    /// <summary>
    /// Test cases for Premium Trie data file.
    /// </summary>
    [TestClass]
    public class Trie : TrieBase
    {
        protected override string DataFile
        {
            get { return FiftyOne.UnitTests.Constants.PREMIUM_TRIE_V32; }
        }

        [TestInitialize]
        public override void Initialise()
        {
            base.Initialise();
        }

        [TestCleanup]
        public void CleanUp()
        {
            base.Dispose();
        }
        
        [TestMethod]
        public void PremiumTrie_Reconcile_Unique()
        {
            base.Reconcile(FiftyOne.UnitTests.UserAgentGenerator.GetUniqueUserAgents());
        }

        [TestMethod]
        public void PremiumTrie_Reconcile_Bad()
        {
            base.Reconcile(FiftyOne.UnitTests.UserAgentGenerator.GetBadUserAgents());
        }
        
        [TestMethod]
        public void PremiumTrie_Reconcile_Random()
        {
            base.Reconcile(FiftyOne.UnitTests.UserAgentGenerator.GetRandomUserAgents());
        }
    }
}
