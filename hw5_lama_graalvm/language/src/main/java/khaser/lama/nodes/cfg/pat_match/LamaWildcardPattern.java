package khaser.lama.nodes.cfg.pat_match;

import java.util.LinkedList;
import java.util.List;

public class LamaWildcardPattern extends LamaPattern {

    public LamaWildcardPattern() {
        super();
    }

    public LamaWildcardPattern(String bindName) {
        super(bindName);
    }

    @Override
    public Boolean checkMatch(Object scrut) {
        return true;
    }

    @Override
    protected List<Binding> getChildBindings(Object scrut) {
        return new LinkedList<>();
    }
}
