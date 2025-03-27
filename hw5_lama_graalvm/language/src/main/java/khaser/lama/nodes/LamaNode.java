package khaser.lama.nodes;

import com.oracle.truffle.api.nodes.Node;
import com.oracle.truffle.api.frame.VirtualFrame;

import khaser.lama.LamaContext;
import khaser.lama.LamaLanguage;

public abstract class LamaNode extends Node {

    protected final LamaContext getContext() {
        return LamaContext.get(this);
    }

    protected final LamaLanguage currentTruffleLanguage() {
        return LamaLanguage.get(this);
    }
    public abstract Object execute(VirtualFrame frame);
}
