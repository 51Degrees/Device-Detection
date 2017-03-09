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
using System.Collections.Generic;

namespace FiftyOne.UnitTests.Performance.Enterprise
{
    [TestClass]
    public class TrieOne : TrieBase
    {
        private static readonly string[] REQUIRED_PROPERTIES = new string[] { "IsMobile" };

        protected override int MaxInitializeTime
        {
            get { return 7500; }
        }

        protected override IEnumerable<string> RequiredProperties
        {
            get
            {
                return REQUIRED_PROPERTIES;
            }
        }

        protected override string DataFile
        {
            get { return Constants.ENTERPRISE_TRIE_V32; }
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
        [TestCategory("Enterprise"), TestCategory("Trie"), TestCategory("Performance"), TestCategory("Initialize")]
        public void EnterpriseV32Trie_Performance_InitializeTimeOne()
        {
            InitializeTime();
        }
        
        [TestMethod]
        [TestCategory("Enterprise"), TestCategory("Trie"), TestCategory("Performance"), TestCategory("Multi"), TestCategory("Bad"), TestCategory("One")]
        public void EnterpriseV32Trie_Performance_BadUserAgentsMultiOne()
        {
            BadUserAgentsMulti(1);
        }

        [TestMethod]
        [TestCategory("Enterprise"), TestCategory("Trie"), TestCategory("Performance"), TestCategory("Single"), TestCategory("Bad"), TestCategory("One")]
        public void EnterpriseV32Trie_Performance_BadUserAgentsSingleOne()
        {
            BadUserAgentsSingle(9);
        }

        [TestMethod]
        [TestCategory("Enterprise"), TestCategory("Trie"), TestCategory("Performance"), TestCategory("Multi"), TestCategory("Unique"), TestCategory("One")]
        public void EnterpriseV32Trie_Performance_UniqueUserAgentsMultiOne()
        {
            UniqueUserAgentsMulti(1);
        }

        [TestMethod]
        [TestCategory("Enterprise"), TestCategory("Trie"), TestCategory("Performance"), TestCategory("Single"), TestCategory("Unique"), TestCategory("One")]
        public void EnterpriseV32Trie_Performance_UniqueUserAgentsSingleOne()
        {
            UniqueUserAgentsSingle(1);
        }

        [TestMethod]
        [TestCategory("Enterprise"), TestCategory("Trie"), TestCategory("Performance"), TestCategory("Multi"), TestCategory("Random"), TestCategory("One")]
        public void EnterpriseV32Trie_Performance_RandomUserAgentsMultiOne()
        {
            RandomUserAgentsMulti(1);
        }

        [TestMethod]
        [TestCategory("Enterprise"), TestCategory("Trie"), TestCategory("Performance"), TestCategory("Single"), TestCategory("Random"), TestCategory("One")]
        public void EnterpriseV32Trie_Performance_RandomUserAgentsSingleOne()
        {
            RandomUserAgentsSingle(1);
        }
    }
}
