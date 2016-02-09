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

namespace FiftyOne.Mobile.Detection.Provider.Interop
{
    /// <summary>
    /// Class used to wrap functions exposed by the tree matching C
    /// DLL.
    /// </summary>
    public class TrieWrapper : IWrapper
    {
        #region Fields

        /// <summary>
        /// The number of instances of the wrapper.
        /// </summary>
        private static int _instanceCount = 0;

        /// <summary>
        /// Used to lock initialise and destroy calls.
        /// </summary>
        private static object _lock = new Object();

        /// <summary>
        /// Collection of property names to indexes.
        /// </summary>
        internal readonly SortedList<string, int> PropertyIndexes = new SortedList<string, int>();

        /// <summary>
        /// The name of the file used to create the current 
        /// single underlying provider.
        /// </summary>
        private static string _fileName;

        /// <summary>
        /// Set to true when dispose has run for the wrapper
        /// as sometimes the finaliser still runs even when
        /// requested not to.
        /// </summary>
        private bool _disposed = false;

        /// <summary>
        /// Provider used by the wrapper.
        /// </summary>
        private readonly Trie.Provider _provider;

        #endregion

        #region Constructor and Destructor

        /// <summary>
        /// Construct the wrapper.
        /// </summary>
        /// <param name="fileName">The full path to the file containing device data.</param>
        public TrieWrapper(string fileName) : this(fileName, String.Empty) { }

        /// <summary>
        /// Construct the wrapper.
        /// </summary>
        /// <param name="fileName">The full path to the file containing device data.</param>
        /// <param name="properties">Array of properties to include in the results.</param>
        public TrieWrapper(string fileName, string[] properties) : this(fileName, String.Join(",", properties)) { }

        /// <summary>
        /// Construct the wrapper.
        /// </summary>
        /// <param name="fileName">The full path to the file containing device data.</param>
        /// <param name="properties">Comma seperated list of properties to include in the results.</param>
        public TrieWrapper(string fileName, string properties)
        {
            lock (_lock)
            {
                // Check the file exists before trying to load it.
                var info = new FileInfo(fileName);
                if (info.Exists == false)
                {
                    throw new ArgumentException(String.Format(
                        "File '{0}' can not be found.",
                        info.FullName), "fileName");
                }

                // If a file has already been loaded then check it's the 
                // same name as the one being used for this instance. Only
                // one file can be loaded at a time.
                if (_fileName != null &&
                    _fileName.Equals(fileName) == false)
                {
                    throw new ArgumentException(String.Format(
                        "Trie has already been initialised with file name '{0}'. " +
                        "Multiple providers with different file sources can not be created.",
                        _fileName), "fileName");
                }

                // Only initialise the memory if the file has not already
                // been loaded into memory.
                if (_fileName == null)
                {
                    _provider = new Trie.Provider(info.FullName, properties);

                    // Initialise the list of property names and indexes.
                    var propertyIndex = 0;
                    foreach (var property in _provider.getAvailableProperties())
                    {
                        PropertyIndexes.Add(property, propertyIndex);
                        propertyIndex++;
                    }

                    // Initialise the list of http header names.
                    foreach (var httpHeader in _provider.getHttpHeaders())
                    {
                        HttpHeaders.Add(httpHeader);
                    }
                    HttpHeaders.Sort();

                    _fileName = fileName;
                }

                // Increase the number of wrapper instances that have
                // been created. Used when the wrapper is disposed to 
                // determine if the memory used should be released.
                _instanceCount++;
            }
        }

        /// <summary>
        /// Ensure the memory used by trie has been freed.
        /// </summary>
        ~TrieWrapper()
        {
            Disposing(false);
        }

        /// <summary>
        /// When disposed of correctly ensures all memory is freed.
        /// </summary>
        public void Dispose()
        {
            Disposing(true);
            GC.SuppressFinalize(true);
        }

        /// <summary>
        /// If the instance count is zero disposes of the memory.
        /// </summary>
        /// <param name="disposing"></param>
        protected virtual void Disposing(bool disposing)
        {
            lock (_lock)
            {
                if (_disposed == false)
                {
                    if (_instanceCount == 1 &&
                        _fileName != null)
                    {
                        // Clear down any static data and free memory.
                        HttpHeaders.Clear();
                        PropertyIndexes.Clear();
                        _fileName = null;
                        _provider.Dispose();
                        _disposed = true;

                        Debug.WriteLine("Freed Trie Data");
                    }
                    _instanceCount--;
                    _disposed = true;
                }
            }
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
        private readonly List<string> _httpHeaders = new List<string>();

        /// <summary>
        /// A list of properties available from the provider.
        /// </summary>
        public IList<string> AvailableProperties
        {
            get { return PropertyIndexes.Keys; }
        }

        /// <summary>
        /// Returns a list of properties and values for the userAgent provided.
        /// </summary>
        /// <param name="userAgent">The useragent to search for.</param>
        /// <returns>A list of properties.</returns>
        public IMatchResult Match(string userAgent)
        {
            return (IMatchResult)_provider.getMatch(userAgent == null ? String.Empty : userAgent);
        }

        public IMatchResult Match(NameValueCollection headers)
        {
            using (var mappedHeaders = new Trie.MapStringString())
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
            throw new NotImplementedException();
        }

        public Pattern.Profiles FindProfiles(string propertyName, string valueName)
        {
            throw new NotImplementedException();
        }

        public Pattern.Profiles FindProfiles(string propertyName, string valueName, Pattern.Profiles profilesList)
        {
            throw new NotImplementedException();
        }

        #endregion
    }
}