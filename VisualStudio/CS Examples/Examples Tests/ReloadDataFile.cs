using System;
using FiftyOne.Example.Illustration.CSharp.Reload_Data_File;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace Examples_Tests
{
    [TestClass]
    class ReloadDataFile
    {
        [TestMethod]
        [TestCategory("CSharpAPIExample"), TestCategory("Lite")]
        public void LiteExamples_Test()
        {
            Program program = new Program(Constants.LITE_PATTERN_V32, 
                                          Constants.GOOD_USERAGENTS_FILE, 
                                          "IsMobile,BrowserName");
        }
        [TestMethod]
        [TestCategory("CSharpAPIExample"), TestCategory("Premium")]
        public void PremiumExamples_Strongly_Typed()
        {
            Program program = new Program(Constants.PREMIUM_PATTERN_V32,
                                          Constants.GOOD_USERAGENTS_FILE,
                                          "IsMobile,BrowserName");
        }
        [TestMethod]
        [TestCategory("CSharpAPIExample"), TestCategory("Enterprise")]
        public void EnterpriseExamples_Strongly_Typed()
        {
            Program program = new Program(Constants.ENTERPRISE_PATTERN_V32,
                                          Constants.GOOD_USERAGENTS_FILE,
                                          "IsMobile,BrowserName");
        }
    }
}
