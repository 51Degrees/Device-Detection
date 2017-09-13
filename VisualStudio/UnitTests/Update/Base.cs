using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;

namespace FiftyOne.UnitTests.Update
{
    public abstract class Base : FiftyOne.UnitTests.Base
    {
        /// <summary>
        /// Determines the reload method to use.
        /// </summary>
        protected enum ReloadMode
        {
            // Reloads the data set from byte array..
            Memory,
            // Reloads the data set from the file.
            File
        }

        private class State
        {
            internal bool Processing = true;
            internal int Reloads = 0;
            internal string WarningMessage = null;
        }

        /// <summary>
        /// Tries to reload the provider with an empty array and should
        /// throw an exception.
        /// </summary>
        protected void UpdateTestEmpty()
        {
            _wrapper.ReloadFromMemory(new byte[0]);
        }

        /// <summary>
        /// Updates (reloads) the data set whilst detection is occurring.
        /// </summary>
        /// <param name="mode">
        /// The method used to reload the data set.
        /// </param>
        protected void UpdateTest(ReloadMode mode)
        {
            var userAgents = UserAgentGenerator.GetUniqueUserAgents();
            var a = UpdateTest(mode, userAgents);
            Console.WriteLine("a: {0}", a);
            var b = UpdateTest(mode, userAgents);
            Console.WriteLine("b: {0}", b);
            Assert.IsTrue(a == b,
                "Two tests with the same User-Agents repeatably reloading the " +
                "data file resulted in different results.");
        }

        private int UpdateTest(ReloadMode mode, IEnumerable<string> userAgents)
        {
            var checkSum = 0;
            var state = new State();
            var updateTask = new Task(() =>
            {
                var random = new Random();
                while (state.Processing)
                {
                    try { 
                        switch (mode)
                        {
                            case ReloadMode.File:
                                _wrapper.ReloadFromFile();
                                break;
                            case ReloadMode.Memory:
                                _wrapper.ReloadFromMemory();
                                break;
                        }
                        state.Reloads++;
                        Thread.Sleep(random.Next(1000));
                    }
                    catch (ApplicationException ex)
                    {
                        if (ex.Message.Equals("Reload from memory not " +
                            "supported with indirect operation."))
                        {
                            state.WarningMessage = ex.Message;
                            state.Processing = false;
                        }
                    }
                }
            });
            try
            {
                updateTask.Start();
                Parallel.ForEach(userAgents, (ua, loopState) =>
                {
                    using (var match = _wrapper.Match(ua))
                    {
                        var hashcode = 0;
                        foreach (var property in _wrapper.AvailableProperties)
                        {
                            foreach (var value in match.getValues(property))
                            {
                                hashcode ^= value.GetHashCode();
                            }
                        }
                        lock (this)
                        {
                            checkSum ^= hashcode;
                        }
                    }
                    if (state.Processing == false)
                    {
                        // If processing should be stopped due to an exception
                        // in the reload then stop 
                        loopState.Stop();
                    }
                });
            }
            finally
            {
                state.Processing = false;
                updateTask.Wait();
                Console.WriteLine("'{0}' Reloads", state.Reloads);
            }
            
            if (state.WarningMessage != null)
            {
                Assert.Inconclusive(state.WarningMessage);
            }

            return checkSum;
        }
    }
}
