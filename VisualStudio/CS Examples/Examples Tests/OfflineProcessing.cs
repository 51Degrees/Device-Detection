using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using FiftyOne.Example.Illustration.CSharp.OfflineProcessing;

namespace Examples_Tests
{
    [TestClass]
    public class OfflineProcessing
    {
        [TestMethod]
        public void Examples_Offline_Processing()
        {
            Program.Run(Constants.LITE_PATTERN_V32, Constants.GOOD_USERAGENTS_FILE);
        }
    }
}
