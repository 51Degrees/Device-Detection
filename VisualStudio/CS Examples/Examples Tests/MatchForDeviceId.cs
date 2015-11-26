using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using FiftyOne.Example.Illustration.CSharp.MatchForDeviceId;

namespace Examples_Tests
{
    [TestClass]
    public class MatchForDeviceId
    {
        [TestMethod]
        public void Examples_Match_For_Device_Id()
        {
            Program.Run(Constants.LITE_PATTERN_V32);
        }
    }
}
