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

namespace FiftyOne.UnitTests.Performance.Enterprise
{
    [TestClass]
    public class TrieAll : TrieBase
    {
        protected override int MaxInitializeTime
        {
            get { return 9000; }
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
        public void EnterpriseV32Trie_Performance_InitializeTimeAll()
        {
            InitializeTime();
        }
        
        [TestMethod]
        [TestCategory("Enterprise"), TestCategory("Trie"), TestCategory("Performance"), TestCategory("Multi"), TestCategory("Bad"), TestCategory("All")]
        public void EnterpriseV32Trie_Performance_BadUserAgentsMultiAll()
        {
            BadUserAgentsMulti(2);
        }

        [TestMethod]
        [TestCategory("Enterprise"), TestCategory("Trie"), TestCategory("Performance"), TestCategory("Single"), TestCategory("Bad"), TestCategory("All")]
        public void EnterpriseV32Trie_Performance_BadUserAgentsSingleAll()
        {
            BadUserAgentsSingle(4);
        }

        [TestMethod]
        [TestCategory("Enterprise"), TestCategory("Trie"), TestCategory("Performance"), TestCategory("Multi"), TestCategory("Unique"), TestCategory("All")]
        public void EnterpriseV32Trie_Performance_UniqueUserAgentsMultiAll()
        {
            UniqueUserAgentsMulti(1);
        }

        [TestMethod]
        [TestCategory("Enterprise"), TestCategory("Trie"), TestCategory("Performance"), TestCategory("Single"), TestCategory("Unique"), TestCategory("All")]
        public void EnterpriseV32Trie_Performance_UniqueUserAgentsSingleAll()
        {
            UniqueUserAgentsSingle(3);
        }

        [TestMethod]
        [TestCategory("Enterprise"), TestCategory("Trie"), TestCategory("Performance"), TestCategory("Multi"), TestCategory("Random"), TestCategory("All")]
        public void EnterpriseV32Trie_Performance_RandomUserAgentsMultiAll()
        {
            RandomUserAgentsMulti(1);
        }

        [TestMethod]
        [TestCategory("Enterprise"), TestCategory("Trie"), TestCategory("Performance"), TestCategory("Single"), TestCategory("Random"), TestCategory("All")]
        public void EnterpriseV32Trie_Performance_RandomUserAgentsSingleAll()
        {
            RandomUserAgentsSingle(3);
        }
    }
}
