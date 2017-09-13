using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using FiftyOne.Example.Illustration.CSharp.GettingStartedTrie;
using FiftyOne.UnitTests;

namespace Examples_Tests
{
    [TestClass]
    public class GettingStartedTrie
    {
        [TestMethod]
        [TestCategory("CSharpAPIExample"), TestCategory("Lite")]
        public void LiteExamples_Getting_Started_Trie()
        {
            Utils.CheckFileExists(Constants.LITE_TRIE_V34);
            Program.Run(Constants.LITE_TRIE_V34);
        }
        [TestMethod]
        [TestCategory("CSharpAPIExample"), TestCategory("Enterprise")]
        public void EnterpriseExamples_Getting_Started_Trie()
        {
            Utils.CheckFileExists(Constants.ENTERPRISE_TRIE_V34);
            Program.Run(Constants.ENTERPRISE_TRIE_V34);
        }
    }
}
