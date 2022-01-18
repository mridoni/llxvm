using System;
using System.Collections.Generic;
using System.Text;

namespace LLXVM.Runtime
{
    public class Error
    {
        /** Not super-user */
        public static int EPERM = 1;
        /** No such file or directory */
        public static int ENOENT = 2;
        /** No such process */
        public static int ESRCH = 3;
        /** interrupted system call */
        public static int EINTR = 4;
        /** I/O error */
        public static int EIO = 5;
        /** No such device or address */
        public static int ENXIO = 6;
        /** Arg list too long */
        public static int E2BIG = 7;
        /** Exec format error */
        public static int ENOEXEC = 8;
        /** Bad file number */
        public static int EBADF = 9;
        /** No children */
        public static int ECHILD = 10;
        /** No more processes */
        public static int EAGAIN = 11;
        /** Not enough core */
        public static int ENOMEM = 12;
        /** Permission denied */
        public static int EACCES = 13;
        /** Bad address */
        public static int EFAULT = 14;
        /** Block device required */
        public static int ENOTBLK = 15;
        /** Mount device busy */
        public static int EBUSY = 16;
        /** File exists */
        public static int EEXIST = 17;
        /** Cross-device link */
        public static int EXDEV = 18;
        /** No such device */
        public static int ENODEV = 19;
        /** Not a directory */
        public static int ENOTDIR = 20;
        /** Is a directory */
        public static int EISDIR = 21;
        /** Invalid argument */
        public static int EINVAL = 22;
        /** Too many open files in system */
        public static int ENFILE = 23;
        /** Too many open files */
        public static int EMFILE = 24;
        /** Not a typewriter */
        public static int ENOTTY = 25;
        /** Text file busy */
        public static int ETXTBSY = 26;
        /** File too large */
        public static int EFBIG = 27;
        /** No space left on device */
        public static int ENOSPC = 28;
        /** Illegal seek */
        public static int ESPIPE = 29;
        /** Read only file system */
        public static int EROFS = 30;
        /** Too many links */
        public static int EMLINK = 31;
        /** Broken pipe */
        public static int EPIPE = 32;
        /** Math arg out of domain of func */
        public static int EDOM = 33;
        /** Math result not representable */
        public static int ERANGE = 34;
        /** No message of desired type */
        public static int ENOMSG = 35;
        /** Identifier removed */
        public static int EIDRM = 36;
        /** Channel number out of range */
        public static int ECHRNG = 37;
        /** Level 2 not synchronized */
        public static int EL2NSYNC = 38;
        /** Level 3 halted */
        public static int EL3HLT = 39;
        /** Level 3 reset */
        public static int EL3RST = 40;
        /** Link number out of range */
        public static int ELNRNG = 41;
        /** Protocol driver not attached */
        public static int EUNATCH = 42;
        /** No CSI structure available */
        public static int ENOCSI = 43;
        /** Level 2 halted */
        public static int EL2HLT = 44;
        /** Deadlock condition */
        public static int EDEADLK = 45;
        /** No record locks available */
        public static int ENOLCK = 46;
        /** Invalid exchange */
        public static int EBADE = 50;
        /** Invalid request descriptor */
        public static int EBADR = 51;
        /** Exchange full */
        public static int EXFULL = 52;
        /** No anode */
        public static int ENOANO = 53;
        /** Invalid request code */
        public static int EBADRQC = 54;
        /** Invalid slot */
        public static int EBADSLT = 55;
        /** File locking deadlock error */
        public static int EDEADLOCK = 56;
        /** Bad font file fmt */
        public static int EBFONT = 57;
        /** Device not a stream */
        public static int ENOSTR = 60;
        /** No data (for no delay io) */
        public static int ENODATA = 61;
        /** Timer expired */
        public static int ETIME = 62;
        /** Out of streams resources */
        public static int ENOSR = 63;
        /** Machine is not on the network */
        public static int ENONET = 64;
        /** Package not installed */
        public static int ENOPKG = 65;
        /** The object is remote */
        public static int EREMOTE = 66;
        /** The link has been severed */
        public static int ENOLINK = 67;
        /** Advertise error */
        public static int EADV = 68;
        /** Srmount error */
        public static int ESRMNT = 69;
        /** Communication error on send */
        public static int ECOMM = 70;
        /** Protocol error */
        public static int EPROTO = 71;
        /** Multihop attempted */
        public static int EMULTIHOP = 74;
        /** Inode is remote (not really error) */
        public static int ELBIN = 75;
        /** Cross mount point (not really error) */
        public static int EDOTDOT = 76;
        /** Trying to read unreadable message */
        public static int EBADMSG = 77;
        /** Inappropriate file type or format */
        public static int EFTYPE = 79;
        /** Given log. name not unique */
        public static int ENOTUNIQ = 80;
        /** f.d. invalid for this operation */
        public static int EBADFD = 81;
        /** Remote address changed */
        public static int EREMCHG = 82;
        /** Can't access a needed shared lib */
        public static int ELIBACC = 83;
        /** Accessing a corrupted shared lib */
        public static int ELIBBAD = 84;
        /** .lib section in a.out corrupted */
        public static int ELIBSCN = 85;
        /** Attempting to link in too many libs */
        public static int ELIBMAX = 86;
        /** Attempting to exec a shared library */
        public static int ELIBEXEC = 87;
        /** Function not implemented */
        public static int ENOSYS = 88;
        /** No more files */
        public static int ENMFILE = 89;
        /** Directory not empty */
        public static int ENOTEMPTY = 90;
        /** File or path name too long */
        public static int ENAMETOOLONG = 91;
        /** Too many symbolic links */
        public static int ELOOP = 92;
        /** Operation not supported on transport endpoint */
        public static int EOPNOTSUPP = 95;
        /** Protocol family not supported */
        public static int EPFNOSUPPORT = 96;
        /** Connection reset by peer */
        public static int ECONNRESET = 104;
        /** No buffer space available */
        public static int ENOBUFS = 105;
        /** Address family not supported by protocol family */
        public static int EAFNOSUPPORT = 106;
        /** Protocol wrong type for socket */
        public static int EPROTOTYPE = 107;
        /** Socket operation on non-socket */
        public static int ENOTSOCK = 108;
        /** Protocol not available */
        public static int ENOPROTOOPT = 109;
        /** Can't send after socket shutdown */
        public static int ESHUTDOWN = 110;
        /** Connection refused */
        public static int ECONNREFUSED = 111;
        /** Address already in use */
        public static int EADDRINUSE = 112;
        /** Connection aborted */
        public static int ECONNABORTED = 113;
        /** Network is unreachable */
        public static int ENETUNREACH = 114;
        /** Network interface is not configured */
        public static int ENETDOWN = 115;
        /** Connection timed out */
        public static int ETIMEDOUT = 116;
        /** Host is down */
        public static int EHOSTDOWN = 117;
        /** Host is unreachable */
        public static int EHOSTUNREACH = 118;
        /** Connection already in progress */
        public static int EINPROGRESS = 119;
        /** Socket already connected */
        public static int EALREADY = 120;
        /** Destination address required */
        public static int EDESTADDRREQ = 121;
        /** Message too long */
        public static int EMSGSIZE = 122;
        /** Unknown protocol */
        public static int EPROTONOSUPPORT = 123;
        /** Socket type not supported */
        public static int ESOCKTNOSUPPORT = 124;
        /** Address not available */
        public static int EADDRNOTAVAIL = 125;
        /** Connection aborted by network */
        public static int ENETRESET = 126;
        /** Socket is already connected */
        public static int EISCONN = 127;
        /** Socket is not connected */
        public static int ENOTCONN = 128;
        /** Too many references: cannot splice */
        public static int ETOOMANYREFS = 129;
        /** Too many processes */
        public static int EPROCLIM = 130;
        /** Too many users */
        public static int EUSERS = 131;
        /** Disk quota exceeded */
        public static int EDQUOT = 132;
        /** Stale file handle */
        public static int ESTALE = 133;
        /** Not supported */
        public static int ENOTSUP = 134;
        /** No medium (in tape drive) */
        public static int ENOMEDIUM = 135;
        /** No such host or network path */
        public static int ENOSHARE = 136;
        /** Filename exists with different case */
        public static int ECASECLASH = 137;
        /** Illegal byte sequence */
        public static int EILSEQ = 138;
        /** Value too large for defined data type */
        public static int EOVERFLOW = 139;
        /** Operation canceled */
        public static int ECANCELED = 140;
        /** State not recoverable */
        public static int ENOTRECOVERABLE = 141;
        /** Previous owner died */
        public static int EOWNERDEAD = 142;

        /** Pointer to errno */
        public static int _errno = Memory.allocateData(4);

        /**
         * Prevent this class from being instantiated.
         */
        private Error() { }

        /**
         * Returns the value of errno.
         * 
         * @return  the value of errno
         */
        public static int errno()
        {
            return Memory.load_i32(_errno);
        }

        /**
         * Sets the value of errno.
         * 
         * @param value  the new value of errno
         * @return       -1
         */
        public static int errno(int value)
        {
            Memory.store(_errno, value);
            return -1;
        }
    }
}
