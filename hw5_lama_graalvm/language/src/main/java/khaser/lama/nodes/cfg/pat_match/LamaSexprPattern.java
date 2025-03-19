package khaser.lama.nodes.cfg.pat_match;

import khaser.lama.LamaContext;
import khaser.lama.nodes.structs.LamaSexpr;

import java.util.LinkedList;
import java.util.List;
import java.util.Objects;

public class LamaSexprPattern extends LamaPattern {

    String ident;
    LamaPattern[] subpats;

    public LamaSexprPattern(String ident, LamaPattern[] subpats) {
        super();
        this.ident = ident;
        this.subpats = subpats;
    }

    @Override
    public Boolean checkMatch(Object scrut) {
        if (scrut instanceof LamaSexpr cscrut) {
            if (cscrut.length() != subpats.length
                || !Objects.equals(cscrut.ident, ident)) return false;
            int n = cscrut.length();
            for (int i = 0; i < n; ++i) {
                if (!subpats[i].checkMatch(cscrut.getEl(i))) return false;
            }
            return true;
        }
        return false;
    }

    @Override
    protected List<Binding> collectBindings(Object scrut) {
        var cscrut = (LamaSexpr) scrut;
        int n = subpats.length;
        var res = new LinkedList<Binding>();
        for (int i = 0; i < n; ++i) {
            res.addAll(subpats[i].collectBindings(cscrut.getEl(i)));
        }
        return res;
    }
}
