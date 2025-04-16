package khaser.lama.nodes.cfg.pat_match;

import java.util.List;

public abstract class LamaPattern {

    abstract public Boolean checkMatch(Object scrut);

    abstract protected List<Binding> collectBindings(Object scrut);
}


