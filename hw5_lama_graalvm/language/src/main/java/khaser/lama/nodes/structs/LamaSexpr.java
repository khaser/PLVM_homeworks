package khaser.lama.nodes.structs;

import java.util.Arrays;

public class LamaSexpr {
    public String ident;
    public Object[] args;

    public LamaSexpr(String ident, Object[] args) {
        this.ident = ident;
        this.args = args;
    }

    @Override
    public boolean equals(Object obj) {
        if (obj instanceof LamaSexpr oth) {
            return this.ident.equals(oth.ident) && Arrays.equals(this.args, oth.args);
        }
        return false;
    }
}
