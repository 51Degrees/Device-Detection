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
        Provider provider;

        public void Run(string fileName)
        {
            // Threads that run device detection in the background.
            int backgroundThreads = 4;
            string properties = "IsMobile";
            Thread[] threads;

            Console.WriteLine("Starting the Reload Data File Example.");
            provider = new Provider(fileName, properties);

            threads = new Thread[backgroundThreads];
            for (int i = 0; i < backgroundThreads; i++)
            {
                threads[i] = new Thread(new ThreadStart(threadPayload));
                threads[i].Start();
            }

            Console.WriteLine(".");

            for (int i = 0; i < backgroundThreads; i++)
            {
                threads[i].Join();
            }

            Console.WriteLine("Complete.");
            Console.ReadKey();
        }

        public static void Main(string[] args)
        {
            string fileName = args.Length > 0 ? args[0] : "../../../../../../data/51Degrees-LiteV3.2.dat";
            Program program = new Program();
            program.Run(fileName);
        }

        public void threadPayload()
        {
            
            string path = "D:\\Workspace\\Device-Detection\\data\\20000 User Agents.csv";
            int hash = 0;
            int recordsProcessed = 0;
            Match match;

            using (FileStream fs = File.Open(path, FileMode.Open, FileAccess.Read, FileShare.ReadWrite))
            using (BufferedStream bs = new BufferedStream(fs))
            using (StreamReader sr = new StreamReader(bs))
            {
                string line;
                while ((line = sr.ReadLine()) != null)
                {
                    match = provider.getMatch(line);
                    hash ^= match.GetHashCode();
                    match.Dispose();
                    recordsProcessed++;
                }
            }
            Console.WriteLine("Thread complete with hash code: " + hash + " and records processed: " + recordsProcessed);
        }
    }
}
