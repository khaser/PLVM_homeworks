package khaser.lama.nodes;

import com.oracle.truffle.api.nodes.Node;
import com.oracle.truffle.api.frame.VirtualFrame;

public abstract class LamaExprNode extends Node {

    public abstract int execInt(VirtualFrame frame);

}
