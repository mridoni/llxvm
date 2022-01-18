/*
* Copyright (c) 2009 David Roberts <d@vidr.cc>
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
 */
package llxvm.runtime;

import java.io.IOException;
import java.lang.System;


// import llxvm.io.FileHandle;
// import llxvm.io.FileSystem;
// import llxvm.io.InputStreamFileHandle;
// import llxvm.io.NativeFileSystem;
// import llxvm.io.OutputStreamFileHandle;
/**
 * Provides utility methods
 *
 * @author Marco Ridoni
 */
public final class Utils {

    public static void jprint_s(int addr) {
        String s = Memory.load_string(addr);
        System.out.print(s);
    }

    public static void jprint_i(int n) {
        String s = Integer.toString(n);
        System.out.print(s);
    }

    public static void jprint_hex(int n) {
        String s = Integer.toHexString(n);
        System.out.print("0x" + s);
    }

    public static void dump_string(String s) {
        String result = "";
        for (byte b : s.getBytes()) {
            result += Integer.toHexString(b) + " ";
        }
        System.out.println(result);

    }
    
    public static void dump_trace()
    {
        try {
            throw new DebugTraceException("");
        }
        catch(Exception ex)
        {
            ex.printStackTrace();
        }
    }
}
