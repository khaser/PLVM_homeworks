package khaser.lama.nodes;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.NodeInfo;

@NodeInfo(shortName = "neg")
public class LamaNegNode extends LamaExprNode {
    @Child
    private LamaExprNode expr;
    public LamaNegNode(LamaExprNode expr) {
        this.expr = expr;
    }

    public Integer execute(VirtualFrame frame) {
        return -this.expr.execute(frame);
    }

}
