package khaser.lama.nodes.structs;

import khaser.lama.LamaContext;

import java.util.Arrays;
import java.util.stream.Collectors;

public class LamaSexpr {
    public String ident;
    public Object[][] args;

    public LamaSexpr(String ident, Object[] args) {
        this.ident = ident;
        this.args = Arrays.stream(args).map(LamaContext::wrapRef).toArray(Object[][]::new);
    }

    @Override
    public boolean equals(Object obj) {
        if (obj instanceof LamaSexpr oth) {
            return this.ident.equals(oth.ident) && Arrays.equals(this.args, oth.args);
        }
        return false;
    }

    @Override
    public String toString() {
        if (args.length > 0) {
            return "%s (%s)".formatted(
                                ident,
                                Arrays.stream(args)
                                        .map(LamaContext::unwrapRef)
                                        .map(el -> el.toString())
                                        .collect(Collectors.joining(", ")));
        } else {
            return ident;
        }
    }
}
