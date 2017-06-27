using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using FiftyOne.Example.Illustration.CSharp.OfflineProcessing;

namespace Examples_Tests
{
    [TestClass]
    public class OfflineProcessing
    {
        [TestMethod]
        [TestCategory("CSharpAPIExample"), TestCategory("Lite")]
        public void LiteExamples_Offline_Processing()
        {
            Program.Run(Constants.LITE_PATTERN_V32, Constants.GOOD_USERAGENTS_FILE);
        }
        [TestMethod]
        [TestCategory("CSharpAPIExample"), TestCategory("Premium")]
        public void PremiumExamples_Offline_Processing()
        {
            Program.Run(Constants.PREMIUM_PATTERN_V32, Constants.GOOD_USERAGENTS_FILE);
        }
        [TestMethod]
        [TestCategory("CSharpAPIExample"), TestCategory("Enterprise")]
        public void EnterpriseExamples_Offline_Processing()
        {
            Program.Run(Constants.ENTERPRISE_PATTERN_V32, Constants.GOOD_USERAGENTS_FILE);
        }
    }
}
