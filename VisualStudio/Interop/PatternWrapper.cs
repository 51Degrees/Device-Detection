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
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.Threading;
using System.Collections.Concurrent;
using System.IO;

namespace FiftyOne.Mobile.Detection.Provider.Interop
{
    /// <summary>
    /// Class used to wrap functions exposed by the pattern matching C
    /// DLL.
    /// </summary>
    public class PatternWrapper : IDisposable
    {
        #region DLL Imports
        
        [DllImport("FiftyOne.Mobile.Detection.Provider.Pattern.dll",
            CallingConvention = CallingConvention.Cdecl, 
            CharSet = CharSet.Ansi)]
        private static extern IntPtr InitWithPropertyString(String fileName, String properties);

        [DllImport("FiftyOne.Mobile.Detection.Provider.Pattern.dll",
            CallingConvention = CallingConvention.Cdecl)]
        private static extern void DataSetFree(IntPtr dataSet);

        [DllImport("FiftyOne.Mobile.Detection.Provider.Pattern.dll",
            CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr ResultsetCacheCreate(IntPtr dataSet, int size);

        [DllImport("FiftyOne.Mobile.Detection.Provider.Pattern.dll",
            CallingConvention = CallingConvention.Cdecl)]
        private static extern void ResultsetCacheFree(IntPtr cache);

        [DllImport("FiftyOne.Mobile.Detection.Provider.Pattern.dll",
            CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr WorksetPoolCreate(IntPtr dataSet, IntPtr cache, int size);

        [DllImport("FiftyOne.Mobile.Detection.Provider.Pattern.dll",
            CallingConvention = CallingConvention.Cdecl)]
        private static extern void WorksetPoolFree(IntPtr pool);

        [DllImport("FiftyOne.Mobile.Detection.Provider.Pattern.dll",
            CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr WorksetPoolGet(IntPtr pool);

        [DllImport("FiftyOne.Mobile.Detection.Provider.Pattern.dll",
            CallingConvention = CallingConvention.Cdecl)]
        private static extern void WorksetPoolRelease(IntPtr pool, IntPtr ws);

        [DllImport("FiftyOne.Mobile.Detection.Provider.Pattern.dll",
            CallingConvention = CallingConvention.Cdecl)]
        private static extern int GetCSVMaxLength(IntPtr ws);

        [DllImport("FiftyOne.Mobile.Detection.Provider.Pattern.dll",
            CallingConvention = CallingConvention.Cdecl,
            CharSet = CharSet.Ansi)]
        private static extern int GetPropertiesCSV(IntPtr ws, String userAgent, StringBuilder result);

        #endregion

        #region Fields

        /// <summary>
        /// A pointer to the dataset to be used by the provider.
        /// </summary>
        private readonly IntPtr DataSet = IntPtr.Zero;

        /// <summary>
        /// Pointer to the pool allocated in the DLL.
        /// </summary>
        private readonly IntPtr Pool;

        /// <summary>
        /// Pointer to the cache allocated in the DLL.
        /// </summary>
        private readonly IntPtr Cache;

        /// <summary>
        /// The length of the data buffer if a CSV.
        /// </summary>
        private readonly int CsvLength;

        #endregion

        #region Constructor and Destructor

        /// <summary>
        /// Construct the wrapper creating a workset for each CPU available.
        /// </summary>
        /// <param name="fileName">Path to the data set file.</param>
        /// <param name="properties">Array of properties to include in the results.</param>
        public PatternWrapper(string fileName, string[] properties) 
            : this(fileName, String.Join(",", properties)) {}

        /// <summary>
        /// Construct the wrapper creating a workset for each CPU available.
        /// </summary>
        /// <param name="fileName">Path to the data set file.</param>
        /// <param name="properties">Comma seperated list of properties to include in the results.
        /// <param name="size">The size of the cache to be used with the wrapper</param>
        /// </param>
        public PatternWrapper(string fileName, string properties, int size = 0)
        {
            var file = new FileInfo(fileName);
            if (file.Exists == false)
            {
                throw new ArgumentException(String.Format(
                    "File '{0}' does not exist", 
                    fileName), 
                    "fileName");
            }
            DataSet = InitWithPropertyString(file.FullName, properties);
            if (DataSet.Equals(0))
            {
                // An error occured creating the dataset. See the result value for the
                // exception.
                switch (Marshal.GetLastWin32Error())
                {
                    case 0:
                        // Everything worked okay and we have a pointer to a valid dataset.
                        break;
                    case 1:
                    default:
                        throw new Exception(String.Format(
                            "Pattern initialisation failed with file '{0}'.",
                            file.FullName));
                }
            }
            CsvLength = GetCSVMaxLength(DataSet);
            Cache = size > 0 ? ResultsetCacheCreate(DataSet, size) : IntPtr.Zero;
            Pool = WorksetPoolCreate(DataSet, Cache, Environment.ProcessorCount);
        }
        
        #endregion

        #region Public Methods

        /// <summary>
        /// Returns a list of key value pairs for the properties.
        /// </summary>
        /// <param name="userAgent">The user agent of the device being searched for.</param>
        /// <returns>The properties and device values for the device matching the useragent provided.</returns>
        public SortedList<string, List<string>> GetProperties(string userAgent)
        {
            return Utils.GetProperties(GetPropertiesAsCSV(userAgent), 0, 1);
        }

        /// <summary>
        /// Returns the properties for the userAgent as a CSV format string.
        /// </summary>
        /// <param name="userAgent"></param>
        /// <returns></returns>
        public StringBuilder GetPropertiesAsCSV(string userAgent)
        {
            var result = new StringBuilder(CsvLength);
            var ws = WorksetPoolGet(Pool);
            try
            {
                result.Capacity = GetPropertiesCSV(ws, userAgent, result);
            }
            finally
            {
                WorksetPoolRelease(Pool, ws);
            }
            return result;
        }

        /// <summary>
        /// Frees all the workset resource created in the DLL.
        /// </summary>
        public void Dispose()
        {
            if (Pool != IntPtr.Zero) { WorksetPoolFree(Pool); }
            if (Cache != IntPtr.Zero) { ResultsetCacheFree(Cache); }
            DataSetFree(DataSet);
        }

        #endregion
    }
}