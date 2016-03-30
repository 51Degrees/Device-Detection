using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using FiftyOne.Mobile.Detection.Provider.Interop.Pattern;
using System.IO;
using System.Threading;

namespace FiftyOne.Example.Illustration.CSharp.Reload_Data_File
{
    class Program
    {
        static Provider provider;
        string deviceDataFile;
        string userAfentsFile;
        string propertiesToUse;
        // Indicates how many threads have finished executing.
        static int threadsFinished = 0;

        public Program(string deviceDataFile, string userAfentsFile, string propertiesToUse)
        {
            this.deviceDataFile = deviceDataFile;
            this.userAfentsFile = userAfentsFile;
            this.propertiesToUse = propertiesToUse;
        }

        public void Run()
        {
            // Threads that run device detection in the background.
            int numberOfThreads = 4;
            // Contains references to background threads.
            Thread[] threads;

            Console.WriteLine("Starting the Reload Data File Example.");

            provider = new Provider(deviceDataFile, propertiesToUse);
            threads = new Thread[numberOfThreads];
            for (int i = 0; i < numberOfThreads; i++)
            {
                threads[i] = new Thread(new ThreadStart(threadPayload));
                threads[i].Start();
            }

            while (threadsFinished < numberOfThreads)
            {
                provider.reloadFromFile();
                Console.WriteLine("Provider reloaded.");
                Thread.Sleep(50);
            }

            for (int i = 0; i < numberOfThreads; i++)
            {
                threads[i].Join();
            }

            // Release resources held by the provider.
            provider.Dispose();
            Console.WriteLine("Threads finished: "+threadsFinished);
            Console.WriteLine("Program execution complete. Press Enter to exit.");
            Console.ReadKey();
        }

        public static void Main(string[] args)
        {
            string fileName = args.Length > 0 ? args[0] : "../../../../../../data/51Degrees-LiteV3.2.dat";
            string userAgents = args.Length > 1 ? args[1] : "../../../../../../data/20000 User Agents.csv";
            string properties = args.Length > 2 ? args[2] : "IsMobile,BrowserName";
            Program program = new Program(fileName, userAgents, properties);
            program.Run();
        }

        public void threadPayload()
        {
            int hash = 0;
            int recordsProcessed = 0;
            Match match;

            using (FileStream fs = File.Open(userAfentsFile, FileMode.Open, FileAccess.Read, FileShare.ReadWrite))
            using (BufferedStream bs = new BufferedStream(fs))
            using (StreamReader sr = new StreamReader(bs))
            {
                string line;
                while ((line = sr.ReadLine()) != null)
                {
                    match = provider.getMatch(line);
                    hash ^= getHash(match);
                    match.Dispose();
                    recordsProcessed++;
                }
            }
            Interlocked.Increment(ref threadsFinished);
            Console.WriteLine("Thread complete with hash code: " + hash + " and records processed: " + recordsProcessed);
        }

        public static int getHash(Match match)
        {
            int hash = -1;
            foreach(var property in provider.getAvailableProperties()) 
            {
                hash += match.getValue(property).GetHashCode();
            }
            return hash;
        }
    }
}
