package khaser.lama;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.PrintStream;
import java.util.HashMap;
import java.util.Map;

import org.graalvm.polyglot.Context;
import org.graalvm.polyglot.PolyglotException;
import org.graalvm.polyglot.Source;
import org.graalvm.polyglot.Value;

public final class LamaMain {

    private static final String Lama = "lama";

    public static void main(String[] args) throws IOException {
        Map<String, String> options = new HashMap<>();

        String filename = null;
        for (String arg : args) {
            if (!parseOption(options, arg)) {
                if (filename == null) {
                    filename = arg;
                } else {
                    System.err.println("Unrecognized option: " + arg);
                }
            }
        }

        if (filename != null) {
            Source source = Source.newBuilder(Lama, new File(filename)).build();
            System.exit(executeSource(source, System.in, System.out, options));
        } else {
            System.err.println("Filename to execute not found");
            System.exit(1);
        }

    }

    private static int executeSource(Source source, InputStream in, PrintStream out, Map<String, String> options) {
        Context context;
        PrintStream err = System.err;
        try {
            context = Context.newBuilder(Lama).in(in).out(out).options(options).allowAllAccess(true).build();
        } catch (IllegalArgumentException e) {
            err.println(e.getMessage());
            return 1;
        }

        try {
            Value result = context.eval(source);
            out.println("Lama successfully executed:");
            out.println(result);
            return 0;
        } catch (PolyglotException ex) {
            if (ex.isInternalError()) {
                // for internal errors we print the full stack trace
                ex.printStackTrace();
            } else {
                err.println(ex.getMessage());
            }
            return 1;
        } finally {
            context.close();
        }
    }
    private static boolean parseOption(Map<String, String> options, String arg) {
        if (arg.length() <= 2 || !arg.startsWith("--")) {
            return false;
        }
        int eqIdx = arg.indexOf('=');
        String key;
        String value;
        if (eqIdx < 0) {
            key = arg.substring(2);
            value = null;
        } else {
            key = arg.substring(2, eqIdx);
            value = arg.substring(eqIdx + 1);
        }

        if (value == null) {
            value = "true";
        }
        int index = key.indexOf('.');
        String group = key;
        if (index >= 0) {
            group = group.substring(0, index);
        }
        options.put(key, value);
        return true;
    }
}
