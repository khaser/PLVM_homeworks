package khaser.lama.nodes.cfg.pat_match;

import khaser.lama.LamaContext;

import java.util.LinkedList;
import java.util.List;

public class LamaArrayPattern extends LamaPattern {

    LamaPattern[] subpats;

    public LamaArrayPattern(LamaPattern[] subpats) {
        super();
        this.subpats = subpats;
    }

    public LamaArrayPattern(LamaPattern[] subpats, String bindName) {
        super(bindName);
        this.subpats = subpats;
    }

    @Override
    public Boolean checkMatch(Object scrut) {
        if (!(scrut instanceof Object[])) return false;
        var cscrut = (Object[]) scrut;
        if (cscrut.length != subpats.length) return false;
        int n = cscrut.length;
        for (int i = 0; i < n; ++i) {
            if (!subpats[i].checkMatch(LamaContext.unwrapRef((Object[]) cscrut[i]))) return false;
        }

        return true;
    }

    @Override
    protected List<Binding> getChildBindings(Object scrut) {
        var cscrut = (Object[]) scrut;
        int n = subpats.length;
        var res = new LinkedList<Binding>();
        for (int i = 0; i < n; ++i) {
            res.addAll(subpats[i].getBindings(LamaContext.unwrapRef((Object[]) cscrut[i])));
        }
        return res;
    }
}
