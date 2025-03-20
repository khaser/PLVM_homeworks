package khaser.lama.nodes.cfg.pat_match;

import khaser.lama.nodes.structs.LamaList;

import java.util.LinkedList;
import java.util.List;

public class LamaConsPattern extends LamaPattern {

    LamaPattern head;
    LamaPattern tail;

    public LamaConsPattern(LamaPattern head, LamaPattern tail) {
        super();
        this.head = head;
        this.tail = tail;
    }

    @Override
    public Boolean checkMatch(Object scrut) {
        if (scrut instanceof LamaList cscrut) {
            if (cscrut.length() == 0) return false;
            return head.checkMatch(cscrut.getEl(0))
                    && tail.checkMatch(cscrut.tail());
        }
        return false;
    }

    @Override
    protected List<Binding> collectBindings(Object scrut) {
        var cscrut = (LamaList) scrut;
        var res = head.collectBindings(cscrut.getEl(0));
        res.addAll(tail.collectBindings(cscrut.tail()));
        return res;
    }
}
