package llxvm.runtime;

import java.lang.Exception;
import java.lang.System;

public class DebugTraceException extends Exception {
        public DebugTraceException(String errorMessage) {
        System.err.println("** No error occurred, just tracing **");
        if (!errorMessage.isEmpty()) 
            System.err.println(errorMessage);
    }
}
