using FiftyOne.Mobile.Detection.Provider.Interop;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace FiftyOne.UnitTests.API
{
    public abstract class Base : FiftyOne.UnitTests.Base
    {
        private string[] InitProperties = {
            "IsMobile",
            "BrowserName",
            "PlatformName" };

        protected virtual void AssertProperties(IWrapper provider)
        {
            foreach (var property in InitProperties)
            {
                Assert.IsTrue(provider.AvailableProperties.Contains(property));
                using (var match = provider.Match(
                    UserAgentGenerator.GetRandomUserAgent(0)))
                {
                    Assert.IsTrue(
                        String.IsNullOrEmpty(
                            match.getValue(property)) == false);
                }
            }
        }

        protected void InitEmptyPropertiesStringTest()
        {
            using (var provider = CreateWrapper(""))
            {
                Assert.IsTrue(provider.AvailableProperties.Count >
                    InitProperties.Count());

                AssertProperties(provider);
            }
        }

        protected void InitPropertiesStringTest()
        {
            using (var provider = CreateWrapper(String.Join(",", InitProperties)))
            {
                Assert.IsTrue(provider.AvailableProperties.Count ==
                    InitProperties.Count());

                AssertProperties(provider);
            }
        }

        protected void InitEmptyPropertiesArrayTest()
        {
            using (var provider = CreateWrapper(new string[0]))
            {
                Assert.IsTrue(provider.AvailableProperties.Count >
                    InitProperties.Count());

                AssertProperties(provider);
            }
        }

        protected void InitPropertiesArrayTest()
        {
            var userAgent = UserAgentGenerator.GetRandomUserAgent(0);
            using (var provider = CreateWrapper(InitProperties))
            {
                Assert.IsTrue(provider.AvailableProperties.Count ==
                    InitProperties.Count());

                AssertProperties(provider);
            }
        }
    }
}
