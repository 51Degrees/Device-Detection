using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using FiftyOne.Example.Illustration.CSharp.MatchForDeviceId;

namespace Examples_Tests
{
    [TestClass]
    public class MatchForDeviceId
    {
        [TestMethod]
        public void Examples_Match_For_Device_Id_Lite()
        {
            Program.Run(Constants.LITE_PATTERN_V32);
        }
        [TestMethod]
        public void Examples_Match_For_Device_Id_Premium()
        {
            Program.Run(Constants.PREMIUM_PATTERN_V32);
        }
        [TestMethod]
        public void Examples_Match_For_Device_Id_Enterprise()
        {
            Program.Run(Constants.ENTERPRISE_PATTERN_V32);
        }
    }
}
