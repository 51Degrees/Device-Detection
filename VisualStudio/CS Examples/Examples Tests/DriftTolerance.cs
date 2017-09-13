using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using FiftyOne.Example.Illustration.CSharp.DriftTolerance;
using FiftyOne.UnitTests;

namespace Examples_Tests
{
    [TestClass]
    public class DriftTolerance
    {
        [TestMethod]
        [TestCategory("CSharpAPIExample"), TestCategory("Lite")]
        public void LiteExamples_Drift_Tolerance()
        {
            Utils.CheckFileExists(Constants.LITE_TRIE_V34);
            Program.Run(Constants.LITE_TRIE_V34);
        }
        [TestMethod]
        [TestCategory("CSharpAPIExample"), TestCategory("Enterprise")]
        public void EnterpriseExamples_Drift_Tolerance()
        {
            Utils.CheckFileExists(Constants.ENTERPRISE_TRIE_V34);
            Program.Run(Constants.ENTERPRISE_TRIE_V34);
        }
    }
}
