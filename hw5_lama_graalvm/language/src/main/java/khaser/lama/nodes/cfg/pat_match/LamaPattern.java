package khaser.lama.nodes.cfg.pat_match;

import java.util.List;
import java.util.Optional;

public abstract class LamaPattern {

    protected String bindName;

    LamaPattern() {
    }

    LamaPattern(String bindName) {
        this.bindName = bindName;
    }

    abstract public Boolean checkMatch(Object scrut);

    public List<Binding> getBindings(Object scrut) {
        assert checkMatch(scrut);
        var res = getChildBindings(scrut);
        if (bindName != null)
            res.add(new Binding(bindName, scrut));
        return res;
    }

    abstract protected List<Binding> getChildBindings(Object scrut);
}

