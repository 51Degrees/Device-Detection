using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using FiftyOne.UnitTests;

namespace UnitTests.API.Enterprise
{
    [TestClass]
    public class Pattern : PatternBase
    {
        [TestMethod]
        public void API_NullUserAgent()
        {
            _wrapper.GetProperties((string)null);
        }

        [TestMethod]
        public void API_EmptyUserAgent()
        {
            _wrapper.GetProperties(String.Empty);
        }

        [TestMethod]
        public void API_LongUserAgent()
        {
            var userAgent = String.Join(" ", UserAgentGenerator.GetEnumerable(10, 10));
            _wrapper.GetProperties(userAgent);
        }

        protected override string DataFile
        {
            get { return Constants.PREMIUM_PATTERN_V32; }
        }
    }
}
