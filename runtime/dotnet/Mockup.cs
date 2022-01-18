using System;
using System.Collections.Generic;
using System.Text;

namespace LLXVM.Runtime
{
    public class Mockup
    {
        public static int printf(int f, int d)
        {
            string s = Memory.load_string(f);
            Console.WriteLine(s);
            return 0;
        }
    }
}
