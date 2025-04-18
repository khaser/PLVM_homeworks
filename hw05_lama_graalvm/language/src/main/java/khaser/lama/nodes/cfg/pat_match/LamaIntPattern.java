package khaser.lama.nodes.cfg.pat_match;

import java.util.LinkedList;
import java.util.List;

public class LamaIntPattern extends LamaPattern {

    Integer expecVal;

    public LamaIntPattern(Integer expecVal) {
        super();
        this.expecVal = expecVal;
    }

    @Override
    public Boolean checkMatch(Object scrut) {
        return scrut.equals(expecVal);
    }

    @Override
    protected List<Binding> collectBindings(Object scrut) {
        return new LinkedList<>();
    }
}
