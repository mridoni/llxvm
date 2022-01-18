using System;
using System.Collections.Generic;
using System.Text;

namespace LLXVM.Runtime
{
    internal class ReflectionUtils
    {
        public static int sizeOf(Type cls)
        {
            if (cls == typeof(bool)) return 1;
        if(cls == typeof(byte))    return 1;
        if(cls == typeof(char))    return 2;
        if(cls == typeof(short))   return 2;
        if(cls == typeof(int))     return 4;
        if(cls == typeof(long))    return 8;
        if(cls == typeof(float))   return 4;
        if(cls == typeof(double))  return 8;
        throw new ArgumentException(
                "Cannot request size of non-primitive type");
    }
}
}
