-assumenosideeffects public class androidx.compose.runtime.ComposerKt {
    void sourceInformation(androidx.compose.runtime.Composer,java.lang.String);
    void sourceInformationMarkerStart(androidx.compose.runtime.Composer,int,java.lang.String);
    void sourceInformationMarkerEnd(androidx.compose.runtime.Composer);
}

# Composer's class initializer doesn't do anything but create an EMPTY object. Marking the
# initializers as having no side effects can help encourage shrinkers to merge/devirtualize Composer
# with ComposerImpl.
-assumenosideeffects public class androidx.compose.runtime.Composer {
    void <clinit>();
}
-assumenosideeffects public class androidx.compose.runtime.ComposerImpl {
    void <clinit>();
}

# Keep all the functions created to throw an exception. We don't want these functions to be
# inlined in any way, which R8 will do by default. The whole point of these functions is to
# reduce the amount of code generated at the call site.
-keepclassmembers,allowshrinking,allowobfuscation class androidx.compose.runtime.** {
    # java.lang.Void == methods that return Nothing
    static void throw*Exception(...);
    static void throw*ExceptionForNullCheck(...);
    static java.lang.Void throw*Exception(...);
    static java.lang.Void throw*ExceptionForNullCheck(...);

    # For functions generating error messages
    static java.lang.String exceptionMessage*(...);
    java.lang.String exceptionMessage*(...);

    static void compose*RuntimeError(...);
    static java.lang.Void compose*RuntimeError(...);
}

# Runtime uses this field to enable group key based stack trace collection.
-assumevalues class androidx.compose.runtime.tooling.ComposeStackTraceMode {
     private static boolean isMinified return true;
}
