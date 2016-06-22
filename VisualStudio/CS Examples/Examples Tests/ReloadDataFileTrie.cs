using System;
using FiftyOne.Example.Illustration.CSharp.Reload_Data_File_Trie;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace Examples_Tests
{
    [TestClass]
    public class ReloadDataFileTrie
    {
        [TestMethod]
        [TestCategory("CSharpAPIExample"), TestCategory("Lite")]
        public void LiteExamples_Reload_Data_File_Trie()
        {
            Program program = new Program(Constants.LITE_TRIE_V32,
                                          Constants.GOOD_USERAGENTS_FILE,
                                          "IsMobile,BrowserName");
            program.Run();
        }
        [TestMethod]
        [TestCategory("CSharpAPIExample"), TestCategory("Premium")]
        public void PremiumExamples_Reload_Data_File_Trie()
        {
            Program program = new Program(Constants.PREMIUM_TRIE_V32,
                                          Constants.GOOD_USERAGENTS_FILE,
                                          "IsMobile,BrowserName");
            program.Run();
        }
        [TestMethod]
        [TestCategory("CSharpAPIExample"), TestCategory("Enterprise")]
        public void EnterpriseExamples_Reload_Data_File_Trie()
        {
            Program program = new Program(Constants.ENTERPRISE_TRIE_V32,
                                          Constants.GOOD_USERAGENTS_FILE,
                                          "IsMobile,BrowserName");
            program.Run();
        }
    }
}
