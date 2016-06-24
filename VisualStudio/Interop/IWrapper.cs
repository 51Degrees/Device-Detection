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
using System.Collections.Specialized;

namespace FiftyOne.Mobile.Detection.Provider.Interop
{
    public interface IWrapper : IDisposable
    {
        /// <summary>
        /// A list of the http headers that the wrapper can use for detection.
        /// </summary>
        List<string> HttpHeaders { get; }

        /// <summary>
        /// A list of the properties available from the wrapper.
        /// </summary>
        IList<string> AvailableProperties { get; }

        /// <summary>
        /// Returns a list of properties and values for the user agent
        /// provided.
        /// </summary>
        /// <param name="userAgent"></param>
        /// <returns></returns>
        IMatchResult Match(string userAgent);

        /// <summary>
        /// Returns a list of properties and values for the HTTP headers
        /// provided.
        /// </summary>
        /// <param name="headers"></param>
        /// <returns></returns>
        IMatchResult Match(NameValueCollection headers);

        /// <summary>
        /// Returns a match result for the device id provided. The device id
        /// would have been returned from a previous matches DeviceId 
        /// property.
        /// </summary>
        /// <param name="deviceId"></param>
        /// <returns></returns>
        IMatchResult MatchForDeviceId(string userAgent);
     
        /// <summary>
        /// Finds the profiles associated with the property name and value 
        /// provided.
        /// </summary>
        /// <param name="propertyName"></param>
        /// <param name="valueName"></param>
        /// <returns></returns>
        Pattern.Profiles FindProfiles(string propertyName, string valueName);

        /// <summary>
        /// Reloads the original file path and associated configuration. Used
        /// to refresh the active data set being used at runtime after the
        /// wrapper has been created.
        /// </summary>
        void ReloadFromFile();

        /// <summary>
        /// Reloads the original file into memory. Used
        /// to refresh the active data set being used at runtime after the
        /// wrapper has been created.
        /// </summary>
        void ReloadFromMemory();
    }
}