/* *********************************************************************
 * This Source Code Form is copyright of 51Degrees Mobile Experts Limited. 
 * Copyright 2015 51Degrees Mobile Experts Limited, 5 Charlotte Close,
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
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.Threading;
using System.IO;
using System.Collections.Specialized;
using System.Diagnostics;
using System.Linq;

namespace FiftyOne.Mobile.Detection.Provider.Interop
{
    /// <summary>
    /// Class used to wrap functions exposed by the pattern matching C
    /// DLL.
    /// </summary>
    public class PatternWrapper : IWrapper
    {
        #region Fields

        /// <summary>
        /// Insance of a class wrapping the unmanaged dataset.
        /// </summary>
        private readonly Pattern.Provider _provider;

        #endregion

        #region Constructor and Destructor

        /// <summary>
        /// Construct the wrapper creating a workset for each CPU available.
        /// </summary>
        /// <param name="fileName">Path to the data set file.</param>
        public PatternWrapper(string fileName) : this(fileName, new string[] { }) { }

        /// <summary>
        /// Construct the wrapper creating a workset for each CPU available.
        /// </summary>
        /// <param name="fileName">Path to the data set file.</param>
        /// <param name="size">The size of the cache to be used with the wrapper</param>
        public PatternWrapper(string fileName, int size = 10000) : this(fileName, new string[] { }, size) { }

        /// <summary>
        /// Construct the wrapper creating a workset for each CPU available.
        /// </summary>
        /// <param name="fileName">Path to the data set file.</param>
        /// <param name="properties">Collection of properties to include in the results.</param>
        /// <param name="size">The size of the cache to be used with the wrapper</param>
        public PatternWrapper(string fileName, IEnumerable<string> properties, int size = 10000) 
            : this(fileName, String.Join(",", properties), size) {}

        /// <summary>
        /// Construct the wrapper creating a workset for each CPU available.
        /// </summary>
        /// <param name="fileName">Path to the data set file.</param>
        /// <param name="properties">Comma seperated list of properties to include in the results.
        /// <param name="size">The size of the cache to be used with the wrapper</param>
        /// </param>
        public PatternWrapper(string fileName, string properties, int size = 10000)
        {
            _provider = new Pattern.Provider(fileName, properties, size, Environment.ProcessorCount);
            _httpHeaders = new List<string>(_provider.getHttpHeaders());
            _availableProperties = new List<string>(_provider.getAvailableProperties());
        }
        
        #endregion

        #region Public Methods

        /// <summary>
        /// A list of the http headers that the wrapper can use for detection.
        /// </summary>
        public List<string> HttpHeaders
        {
            get { return _httpHeaders; }
        }
        private readonly List<string> _httpHeaders;

        /// <summary>
        /// A list of properties available from the provider.
        /// </summary>
        public IList<string> AvailableProperties
        {
            get { return _availableProperties; }
        }
        private readonly List<string> _availableProperties;

        /// <summary>
        /// Returns a match result for the user agent provided.
        /// </summary>
        /// <param name="userAgent"></param>
        /// <returns></returns>
        public IMatchResult Match(string userAgent)
        {
            return (IMatchResult)_provider.getMatch(userAgent == null ? String.Empty : userAgent);
        }

        /// <summary>
        /// Returns a match result for the headers provided.
        /// </summary>
        /// <param name="headers"></param>
        /// <returns></returns>
        public IMatchResult Match(NameValueCollection headers)
        {
            using (var mappedHeaders = new Pattern.MapStringString())
            {
                foreach (var header in HttpHeaders)
                {
                    var value = headers[header];
                    if (String.IsNullOrEmpty(value) == false)
                    {
                        mappedHeaders.Add(header, value);
                    }
                }
                return (IMatchResult)_provider.getMatch(mappedHeaders);
            }
        }

        /// <summary>
        /// Returns a match result for the device id provided. The device id
        /// would have been returned from a previous matches DeviceId 
        /// property.
        /// </summary>
        /// <param name="deviceId"></param>
        /// <returns></returns>
        public IMatchResult MatchForDeviceId(string deviceId)
        {
            return (IMatchResult)_provider.getMatchForDeviceId(deviceId);
        }

        /// <summary>
        /// Finalize to ensure that all resources have been freed.
        /// </summary>
        ~PatternWrapper()
        {
            Dispose(false);
        }

        /// <summary>
        /// Frees all the unmanaged resources.
        /// </summary>
        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        /// <summary>
        /// Fress all the unmanaged resources.
        /// </summary>
        /// <param name="disposing"></param>
        protected virtual void Dispose(bool disposing)
        {
            if (_provider != null)
            {
                _provider.Dispose();
            }
        }

        #endregion
    }
}