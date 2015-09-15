/* *********************************************************************
 * This Source Code Form is copyright of 51Degrees Mobile Experts Limited. 
 * Copyright © 2014 51Degrees Mobile Experts Limited, 5 Charlotte Close,
 * Caversham, Reading, Berkshire, United Kingdom RG4 7BY
 * 
 * This Source Code Form is the subject of the following patent 
 * applications, owned by 51Degrees Mobile Experts Limited of 5 Charlotte
 * Close, Caversham, Reading, Berkshire, United Kingdom RG4 7BY: 
 * European Patent Application No. 13192291.6; and
 * United States Patent Application Nos. 14/085,223 and 14/085,301.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0.
 * 
 * If a copy of the MPL was not distributed with this file, You can obtain
 * one at http://mozilla.org/MPL/2.0/.
 * 
 * This Source Code Form is “Incompatible With Secondary Licenses”, as
 * defined by the Mozilla Public License, v. 2.0.
 * ********************************************************************* */

using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using FiftyOne.Mobile.Detection.Provider.Interop;
using System.Collections.Generic;
using System.Threading.Tasks;
using FiftyOne.Foundation.Mobile.Detection;
using FiftyOne.Foundation.Mobile.Detection.Factories;

namespace FiftyOne.Reconcile
{
    [TestClass]
    public abstract class TrieBase
    {
        protected abstract string DataFile { get ; }

        protected TrieWrapper _unmanagedProvider;
        protected TrieProvider _managedProvider;

        /// <summary>
        /// Create the two providers for managed and unmanaged code.
        /// </summary>
        public virtual void Initialise()
        {
            _unmanagedProvider = new TrieWrapper(DataFile);
            _managedProvider = TrieFactory.Create(DataFile);
        }

        /// <summary>
        /// Performs detection using the enumeration of user agents
        /// and compares the results for unmanaged and managed detection.
        /// The first difference is reports as a failure.
        /// </summary>
        /// <param name="userAgents">Enumeration of user agents to use for reconciliation</param>
        protected void Reconcile(IEnumerable<string> userAgents)
        {
            Parallel.ForEach(userAgents, userAgent =>
            {
                var managedMatch = _managedProvider.GetDeviceIndex(userAgent.Trim());
                var managedMatchUserAgent = _managedProvider.GetUserAgent(userAgent.Trim());
                using (TrieWrapper.MatchResult unmanagedMatch =
                    (TrieWrapper.MatchResult)_unmanagedProvider.Match(userAgent.Trim()))
                {
                    // Does not use Equals in order to handle situatons where one
                    // or both of the UserAgent properties are null.
                    if (managedMatchUserAgent != unmanagedMatch.UserAgent)
                    {
                        Assert.Fail(String.Format(
                            "Different user agents for target user agent '{0}'.\r\n" +
                            "Managed UA: '{1}'\r\n" +
                            "Unmanaged UA: '{2}'",
                            userAgent,
                            managedMatchUserAgent,
                            unmanagedMatch.UserAgent));
                    }
                    foreach (var property in _managedProvider.PropertyNames)
                    {
                        var managedValue = _managedProvider.GetPropertyValue(managedMatch, property);
                        var unmanagedValue = unmanagedMatch[property];
                        if (managedValue.Equals(unmanagedValue) == false)
                        {
                            Assert.Fail(String.Format(
                                "Different results for property '{0}'.\r\n" +
                                "Managed value: '{1}'\r\n" +
                                "Unmanaged value: '{2}'",
                                property,
                                managedValue,
                                unmanagedValue));
                        }
                    }
                }
            });
        }

        /// <summary>
        /// Disposes of the providers.
        /// </summary>
        public void Dispose()
        {
            Disposing(true);
            GC.SuppressFinalize(this);
        }

        /// <summary>
        /// Ensures any unmanaged memory is freed.
        /// </summary>
        ~TrieBase()
        {
            Disposing(false);
        }

        /// <summary>
        /// Disposes of the providers if not already done so.
        /// </summary>
        /// <param name="disposing"></param>
        protected virtual void Disposing(bool disposing) 
        {
            if (_managedProvider != null)
            {
                _managedProvider.Dispose();
                _managedProvider = null;
            }
            if (_unmanagedProvider != null)
            {
                _unmanagedProvider.Dispose();
                _unmanagedProvider = null;
            }
        }
    }
}
