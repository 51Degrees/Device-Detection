using System;
using FiftyOne.Example.Illustration.CSharp.Reload_From_Memory;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using FiftyOne.UnitTests;

namespace Examples_Tests
{
    [TestClass]
    public class ReloadFromMemory
    {
        [TestMethod]
        [TestCategory("CSharpAPIExample"), TestCategory("Lite")]
        public void LiteExamples_Reload_From_Memory()
        {
            Utils.CheckFileExists(Constants.LITE_PATTERN_V32);
            Utils.CheckFileExists(Constants.GOOD_USERAGENTS_FILE);
            Program program = new Program(Constants.LITE_PATTERN_V32,
                                          Constants.GOOD_USERAGENTS_FILE,
                                          "IsMobile,BrowserName");
            program.Run();
        }
        [TestMethod]
        [TestCategory("CSharpAPIExample"), TestCategory("Premium")]
        public void PremiumExamples_Reload_From_Memory()
        {
            Utils.CheckFileExists(Constants.PREMIUM_PATTERN_V32);
            Utils.CheckFileExists(Constants.GOOD_USERAGENTS_FILE);
            Program program = new Program(Constants.PREMIUM_PATTERN_V32,
                                          Constants.GOOD_USERAGENTS_FILE,
                                          "IsMobile,BrowserName");
            program.Run();
        }
        [TestMethod]
        [TestCategory("CSharpAPIExample"), TestCategory("Enterprise")]
        public void EnterpriseExamples_Reload_From_Memory()
        {
            Utils.CheckFileExists(Constants.ENTERPRISE_PATTERN_V32);
            Utils.CheckFileExists(Constants.GOOD_USERAGENTS_FILE);
            Program program = new Program(Constants.ENTERPRISE_PATTERN_V32,
                                          Constants.GOOD_USERAGENTS_FILE,
                                          "IsMobile,BrowserName");
            program.Run();
        }
    }
}
