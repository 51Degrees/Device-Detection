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
using FiftyOne.Mobile.Detection.Provider.Interop;

namespace FiftyOne.UnitTests.Performance
{
    /// <summary>
    /// Base class for all pattern performance tests.
    /// </summary>
    public abstract class PatternBase : Base
    {
        /// <summary>
        /// Creates a new pattern wrapper recording the time taken
        /// to construct the wrapper.
        /// </summary>
        /// <returns>A new pattern wrapper instance</returns>
        protected override IWrapper CreateWrapper()
        {
            var start = DateTime.UtcNow;
            try
            {
                Utils.CheckFileExists(DataFile);
                return new PatternWrapper(DataFile, String.Empty, 5000);
            }
            finally
            {
                _testInitializeTime = DateTime.UtcNow - start;
            }
        }
    }
}
