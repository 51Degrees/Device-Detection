using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using FiftyOne.Example.Illustration.CSharp.FindProfiles;
using FiftyOne.UnitTests;

namespace Examples_Tests
{
    [TestClass]
    public class FindProfiles
    {
        [TestMethod]
        [TestCategory("CSharpAPIExample"), TestCategory("Lite")]
        public void LiteExamples_Find_Profiles()
        {
            Utils.CheckFileExists(Constants.LITE_PATTERN_V32);
            Program.Run(Constants.LITE_PATTERN_V32);
        }
        [TestMethod]
        [TestCategory("CSharpAPIExample"), TestCategory("Premium")]
        public void PremiumExamples_Find_Profiles()
        {
            Utils.CheckFileExists(Constants.PREMIUM_PATTERN_V32);
            Program.Run(Constants.PREMIUM_PATTERN_V32);
        }
        [TestMethod]
        [TestCategory("CSharpAPIExample"), TestCategory("Enterprise")]
        public void EnterpriseExamples_Find_Profiles()
        {
            Utils.CheckFileExists(Constants.ENTERPRISE_PATTERN_V32);
            Program.Run(Constants.ENTERPRISE_PATTERN_V32);
        }
    }
}
