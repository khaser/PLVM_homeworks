package khaser.lama.nodes.structs;

import com.oracle.truffle.api.CompilerDirectives;
import khaser.lama.LamaContext;

import java.util.Arrays;
import java.util.List;

public class LamaArray {
    protected final Object[][] els;

    public LamaArray(Object[] els) {
        this.els = new Object[els.length][1];
        for (int i = 0; i < els.length; ++i) {
            this.els[i] = LamaContext.wrapRef(els[i]);
        }
    }

    public Object getEl(int idx) {
        Object[] ref = els[idx];
        return LamaContext.unwrapRef(ref);
    }

    public Object getRef(int idx) {
        return els[idx];
    }

    public int length() {
        return els.length;
    }

    @Override
    public boolean equals(Object obj) {
        if (obj instanceof LamaArray oth) {
            return Arrays.equals(this.els, oth.els);
        }
        return false;
    }

    @Override
    public String toString() {
        return Arrays.toString(Arrays.stream(els).map(LamaContext::unwrapRef).toArray());
    }
}
