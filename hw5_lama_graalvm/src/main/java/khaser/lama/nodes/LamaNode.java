package khaser.lama.nodes;

import com.oracle.truffle.api.nodes.Node;
import com.oracle.truffle.api.frame.VirtualFrame;

import khaser.lama.LamaContext;

public abstract class LamaNode extends Node {

    protected final LamaContext getContext() {
        return LamaContext.get(this);
    }

    public abstract Integer execute(VirtualFrame frame);
}
