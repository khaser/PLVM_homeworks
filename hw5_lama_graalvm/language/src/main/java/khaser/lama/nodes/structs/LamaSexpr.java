package khaser.lama.nodes.structs;

import com.oracle.truffle.api.CompilerDirectives;
import khaser.lama.LamaContext;

import java.util.Arrays;
import java.util.stream.Collectors;

public class LamaSexpr extends LamaArray {
    public String ident;

    public LamaSexpr(String ident, Object[] args) {
        super(args);
        this.ident = ident;
    }

    @Override
    public boolean equals(Object obj) {
        if (obj instanceof LamaSexpr oth) {
            return this.ident.equals(oth.ident) && super.equals(obj);
        }
        return false;
    }

    @Override
    public String toString() {
        if (els.length > 0) {
            return "%s (%s)".formatted(
                                ident,
                                Arrays.stream(els)
                                        .map(LamaContext::unwrapRef)
                                        .map(Object::toString)
                                        .collect(Collectors.joining(", ")));
        } else {
            return ident;
        }
    }
}
