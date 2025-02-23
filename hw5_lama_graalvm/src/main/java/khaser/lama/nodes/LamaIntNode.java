package khaser.lama.nodes;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.NodeInfo;

@NodeInfo(shortName = "const value")
public class LamaIntNode extends LamaExprNode {

    private int val;

    public LamaIntNode(int val) {
        this.val = val;
    }

    @Override
    public int execInt(VirtualFrame frame) {
        return val;
    }

    // TODO: correct error handling
    // @Fallback
    // protected Object handleError(Object left, Object right) {
    // }

}
