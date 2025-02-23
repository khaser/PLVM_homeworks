package khaser.lama;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
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
        assert args.length == 2 : "Lama truffle only accepts single file";
        Source source;

        source = Source.newBuilder(Lama, new File(args[0])).build();

        System.exit(executeSource(source, System.in, System.out));
    }

    private static int executeSource(Source source, InputStream in, PrintStream out) {
        Context context;
        PrintStream err = System.err;
        try {
            context = Context.newBuilder(Lama).in(in).out(out).allowAllAccess(true).build();
        } catch (IllegalArgumentException e) {
            err.println(e.getMessage());
            return 1;
        }

        try {
            Value result = context.eval(source);
            out.println("Lama successfully executed:");
            out.println(result);
            // if (context.getBindings(Lama).getMember("main") == null) {
            //     err.println("No function main() defined in Lama source file.");
            //     return 1;
            // }
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

}
