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
using FiftyOne.Foundation.Mobile.Detection;
using FiftyOne.Foundation.Mobile.Detection.Factories;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace FiftyOne.Reconcile
{
    [TestClass]
    public abstract class PatternBase
    {
        protected abstract string DataFile { get ; }

        protected PatternWrapper _unmanagedProvider;
        protected Provider _managedProvider;

        /// <summary>
        /// Create the two providers for managed and unmanaged code.
        /// </summary>
        public virtual void Initialise()
        {
            _unmanagedProvider = new PatternWrapper(DataFile, String.Empty, 5000);
            _managedProvider = new Provider(StreamFactory.Create(DataFile, false));
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
                var managedMatch = _managedProvider.Match(userAgent.Trim());
                using (PatternWrapper.MatchResult unmanagedMatch =
                    (PatternWrapper.MatchResult)_unmanagedProvider.Match(userAgent.Trim()))
                {
                    // Does not use Equals in order to handle situatons where one
                    // or both of the UserAgent properties are null.
                    if (managedMatch.UserAgent != unmanagedMatch.UserAgent)
                    {
                        Assert.Fail(String.Format(
                            "Different user agents for target user agent '{0}'.\r\n" +
                            "Managed UA: '{1}'\r\n" +
                            "Unmanaged UA: '{2}'",
                            userAgent,
                            managedMatch.UserAgent,
                            unmanagedMatch.UserAgent));
                    }
                    if (managedMatch.DeviceId.Equals(
                        unmanagedMatch.DeviceId) == false)
                    {
                        Assert.Fail(String.Format(
                            "Different device ids for user agent '{0}'.\r\n" +
                            "Managed Id: '{1}'\r\n" +
                            "Unmanaged Id: '{2}'",
                            userAgent,
                            managedMatch.DeviceId,
                            unmanagedMatch.DeviceId));
                    }
                    foreach (var property in _managedProvider.DataSet.Properties)
                    {
                        var managedValue = managedMatch[property].ToString();
                        var unmanagedValue = unmanagedMatch[property.Name];
                        if (managedValue.Equals(unmanagedValue) == false)
                        {
                            Assert.Fail(String.Format(
                                "Different results for property '{0}'.\r\n" +
                                "Managed value: '{1}'\r\n" +
                                "Unmanaged value: '{2}'",
                                property.Name,
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
        ~PatternBase()
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
                _managedProvider.DataSet.Dispose();
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
