/* *********************************************************************
 * This Source Code Form is copyright of 51Degrees Mobile Experts Limited.
 * Copyright 2017 51Degrees Mobile Experts Limited, 5 Charlotte Close,
 * Caversham, Reading, Berkshire, United Kingdom RG4 7BY
 *
 * This Source Code Form is the subject of the following patents and patent
 * applications, owned by 51Degrees Mobile Experts Limited of 5 Charlotte
 * Close, Caversham, Reading, Berkshire, United Kingdom RG4 7BY:
 * European Patent No. 2871816;
 * European Patent Application No. 17184134.9;
 * United States Patent Nos. 9,332,086 and 9,350,823; and
 * United States Patent Application No. 15/686,066.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0.
 *
 * If a copy of the MPL was not distributed with this file, You can obtain
 * one at http://mozilla.org/MPL/2.0/.
 *
 * This Source Code Form is "Incompatible With Secondary Licenses", as
 * defined by the Mozilla Public License, v. 2.0.
 ********************************************************************** */

using FiftyOne.Mobile.Detection.Provider.Interop;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace FiftyOne.UnitTests.Performance
{
    /// <summary>
    /// Base class for all trie performance tests.
    /// </summary>
    public abstract class TrieBase : Base
    {
        /// <summary>
        /// Creates a new trie wrapper recording the time taken
        /// to construct the wrapper.
        /// </summary>
        /// <returns>A new trie wrapper instance</returns>
        protected override IWrapper CreateWrapper()
        {
            var start = DateTime.UtcNow;
            try
            {
                Utils.CheckFileExists(DataFile);
                return new TrieWrapper(DataFile, String.Empty);
            }
            finally
            {
                _testInitializeTime = DateTime.UtcNow - start;
            }
        }
    }
}
