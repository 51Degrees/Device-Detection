using System;
using FiftyOne.Example.Illustration.CSharp.Reload_Data_File_Trie;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using FiftyOne.UnitTests;

namespace Examples_Tests
{
    [TestClass]
    public class ReloadDataFileTrie
    {
        [TestMethod]
        [TestCategory("CSharpAPIExample"), TestCategory("Lite")]
        public void LiteExamples_Reload_Data_File_Trie()
        {
            Utils.CheckFileExists(Constants.LITE_TRIE_V34);
            Utils.CheckFileExists(Constants.GOOD_USERAGENTS_FILE);
            Program program = new Program(Constants.LITE_TRIE_V34,
                                          Constants.GOOD_USERAGENTS_FILE,
                                          "IsMobile,BrowserName");
            program.Run();
        }
        [TestMethod]
        [TestCategory("CSharpAPIExample"), TestCategory("Enterprise")]
        public void EnterpriseExamples_Reload_Data_File_Trie()
        {
            Utils.CheckFileExists(Constants.ENTERPRISE_TRIE_V34);
            Utils.CheckFileExists(Constants.GOOD_USERAGENTS_FILE);
            Program program = new Program(Constants.ENTERPRISE_TRIE_V34,
                                          Constants.GOOD_USERAGENTS_FILE,
                                          "IsMobile,BrowserName");
            program.Run();
        }
    }
}
