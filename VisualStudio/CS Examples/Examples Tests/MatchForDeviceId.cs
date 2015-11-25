using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Match_For_Device_Id;

namespace Examples_Tests
{
    [TestClass]
    public class MatchForDeviceId
    {
        [TestMethod]
        public void Examples_Match_For_Device_Id()
        {
            PrivateType ExampleClass = new PrivateType(typeof(Program));
            ExampleClass.InvokeStatic("run", new object[] { Constants.LITE_PATTERN_V32 });
        }
    }
}
