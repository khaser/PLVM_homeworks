package khaser.lama.nodes.cfg.pat_match;

import java.util.LinkedList;
import java.util.List;

public class LamaWildcardPattern extends LamaPattern {

    public LamaWildcardPattern() {
        super();
    }

    @Override
    public Boolean checkMatch(Object scrut) {
        return true;
    }

    @Override
    protected List<Binding> collectBindings(Object scrut) {
        return new LinkedList<>();
    }
}
