package khaser.lama.nodes.cfg.pat_match;

import khaser.lama.LamaContext;
import khaser.lama.nodes.structs.LamaList;

import java.util.LinkedList;
import java.util.List;

public class LamaListPattern extends LamaPattern {

    LamaPattern[] subpats;

    public LamaListPattern(LamaPattern[] subpats) {
        super();
        this.subpats = subpats;
    }

    @Override
    public Boolean checkMatch(Object scrut) {
        if (scrut instanceof LamaList cscrut) {
            if (cscrut.length() != subpats.length) return false;
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
        var cscrut = (LamaList) scrut;
        int n = subpats.length;
        var res = new LinkedList<Binding>();
        for (int i = 0; i < n; ++i) {
            res.addAll(subpats[i].collectBindings(cscrut.getEl(i)));
        }
        return res;
    }
}
