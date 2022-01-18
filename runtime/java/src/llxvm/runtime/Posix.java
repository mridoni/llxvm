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

import java.lang.UnsupportedOperationException;
import java.io.File;

import llxvm.runtime.Memory;

/**
 * Provides routines defined by the POSIX API.
 *
 * @author David Roberts
 */
public final class Posix {

    /**
     * Prevent this class from being instantiated.
     */
    private Posix() {
    }

    /**
     * Manipulate the given file descriptor.
     *
     * @param fd the file descriptor to manipulate
     * @param cmd specifies the operation to perform
     * @param args a pointer to the packed list of varargs i.e. a pointer to the
     * arg argument (if applicable)
     * @return the appropriate value on success, -1 on error
     */
    public static int fcntl(int fd, int cmd, int args) {
        // TODO: implement
        throw new UnsupportedOperationException("fcntl");
//        return Error.errno(Error.EACCES);
    }

    /**
     * Get the time.
     *
     * @param tv a pointer to the timeval structure to set
     * @param tz a pointer to the timezone structure to set
     * @return 0 on success, -1 on error
     */
    public static int gettimeofday(int tv, int tz) {
        // TODO: implement
        throw new UnsupportedOperationException("gettimeofday");
//        return Error.errno(Error.EINVAL);
    }

    /**
     * Examine and change blocked signals.
     *
     * @param how specifies the behaviour of the call
     * @param set specifies how to change the blocked signals
     * @param oldset where to store the previous value of the signal mask
     * @return 0 on success, -1 on error
     */
    public static int sigprocmask(int how, int set, int oldset) {
        // TODO: implement
        throw new UnsupportedOperationException("sigprocmask");
//        return Error.errno(Error.EINVAL);
    }

    /**
     * Get configuration information.
     *
     * @param name the name of the variable to retrieve
     * @return the value of the system resource on success, -1 on error
     */
    public static int sysconf(int name) {
        // TODO: implement
        throw new UnsupportedOperationException("sysconf");
//        return Error.errno(Error.EINVAL);
    }

    public static int mkdir(int name, int mode) {
        // TODO: implement
        throw new UnsupportedOperationException("mkdir");
//        return Error.errno(Error.EINVAL);
    }

    public static int rmdir(int name) {
        // TODO: implement
        throw new UnsupportedOperationException("rmdir");
//        return Error.errno(Error.EINVAL);
    }

    public static int popen(int command, int type) {
        // TODO: implement
        throw new UnsupportedOperationException("popen");
//        return Error.errno(Error.EINVAL);
    }

    public static int pclose(int file) {
        // TODO: implement
        throw new UnsupportedOperationException("pclose");
//        return Error.errno(Error.EINVAL);
    }

    public static int __isinff(float f) {
        // TODO: implement
        throw new UnsupportedOperationException("__isinff");
//        return Error.errno(Error.EINVAL);
    }

    public static int __isnanf(float f) {
        // TODO: implement
        throw new UnsupportedOperationException("__isnanf");
//        return Error.errno(Error.EINVAL);
    }

    public static int access(int addr, int mode) {
        // TODO: implement
        String filename = Memory.load_string(addr);
        File f = new File(filename);
        return f.exists() ? 0 : -1;
//        return Error.errno(Error.EINVAL);
    }

    public static int fdatasync(int i1) {
        // TODO: implement
        throw new UnsupportedOperationException("mkdir");
//        return Error.errno(Error.EINVAL);
    }

    public static void flockfile(int i1) {
        // TODO: implement
        throw new UnsupportedOperationException("fdatasync");
//        return;
    }

    public static void funlockfile(int i1) {
        // TODO: implement
        throw new UnsupportedOperationException("funlockfile");
//        return;
    }

    public static int getlogin() {
        int addr = Memory.storeData("testuser");
        return addr;
    }
    
    public static short getuid() {
        // TODO: implement
        return 1;
    }    
    
    public static int fsync(int fd) {
        // TODO: implement
        throw new UnsupportedOperationException("fsync");
//        return -1;
    }      

    public static int lt_dlclose(int i1) {
        // TODO: implement
        throw new UnsupportedOperationException("lt_dlclose");
//        return Error.errno(Error.EINVAL);
    }

    public static int lt_dlexit() {
        // TODO: implement
        return 0;
//        return Error.errno(Error.EINVAL);
    }

    public static int lt_dlinit() {
        // TODO: implement
        return 0;
//        return Error.errno(Error.EINVAL);
    }

    public static int lt_dlopen(int i1) {
        // TODO: implement
        return 0;
//        return Error.errno(Error.EINVAL);
    }

    public static int lt_dlsym(int i1, int i2) {
        // TODO: implement
        throw new UnsupportedOperationException("lt_dlsym");
//        return Error.errno(Error.EINVAL);
    }

    public static int readlink(int i1, int i2, int i3) {
        // TODO: implement
        throw new UnsupportedOperationException("readlink");
//        return Error.errno(Error.EINVAL);
    }

    public static int sleep(int i1) {
        // TODO: implement
        throw new UnsupportedOperationException("sleep");
//        return Error.errno(Error.EINVAL);
    }

    public static int waitpid(int i1, int i2, int i3) {
        // TODO: implement
        throw new UnsupportedOperationException("waitpid");
//        return Error.errno(Error.EINVAL);
    }

}
