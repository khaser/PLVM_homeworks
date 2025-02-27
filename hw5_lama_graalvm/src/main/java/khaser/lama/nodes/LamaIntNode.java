package khaser.lama.nodes;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.NodeInfo;

@NodeInfo(shortName = "const value")
public final class LamaIntNode extends LamaExprNode {

    private final int val;

    public LamaIntNode(int val) {
        this.val = val;
    }

    @Override
    public Integer execute(VirtualFrame frame) {
        return val;
    }

}
