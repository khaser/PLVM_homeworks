package khaser.lama.nodes.cfg.pat_match;

import java.util.List;

public class LamaBindPattern extends LamaPattern {

    String bindName;
    LamaPattern subpat;

    public LamaBindPattern(String bindName, LamaPattern subpat) {
        this.bindName = bindName;
        this.subpat = subpat;
    }

    @Override
    public Boolean checkMatch(Object scrut) {
        return subpat.checkMatch(scrut);
    }

    @Override
    protected List<Binding> collectBindings(Object scrut) {
        assert checkMatch(scrut);
        var res = subpat.collectBindings(scrut);
        if (bindName != null)
            res.add(new Binding(bindName, scrut));
        return res;
    }
}
