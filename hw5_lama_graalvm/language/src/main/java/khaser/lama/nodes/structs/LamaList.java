package khaser.lama.nodes.structs;

import khaser.lama.LamaContext;

import java.util.LinkedList;
import java.util.Collections;
import java.util.stream.Collectors;
import java.util.stream.Stream;

public class LamaList {
    protected final LinkedList<Object[]> els;

    public LamaList(Stream<Object> els_stream) {
        this.els = els_stream.map(LamaContext::wrapRef)
                             .collect(Collectors.toCollection(LinkedList::new));
    }

    private LamaList(LinkedList<Object[]> els) {
        this.els = els;
    }

    public LamaList tail() {
        var res = new LinkedList<>(els);
        res.removeFirst();
        return new LamaList(res);
    }

    public LamaList cons(Object el) {
        var res = new LinkedList<>(els);
        res.addFirst(LamaContext.wrapRef(el));
        return new LamaList(res);
    }

    public int length() {
        return els.size();
    }

    public Object getEl(int idx) {
        return LamaContext.unwrapRef(els.get(idx));
    }

    @Override
    public boolean equals(Object obj) {
        if (obj instanceof LamaList oth) {
            return this.els.equals(oth.els);
        }
        return false;
    }

    @Override
    public String toString() {
        return "{%s}".formatted(
                        els.stream().map(LamaContext::unwrapRef)
                                    .map(Object::toString)
                                    .collect(Collectors.joining(", ")));
    }
}
