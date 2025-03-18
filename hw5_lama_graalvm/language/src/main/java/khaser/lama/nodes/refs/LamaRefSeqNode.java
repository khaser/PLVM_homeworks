package khaser.lama.nodes.refs;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.NodeInfo;
import khaser.lama.nodes.LamaExprNode;
import khaser.lama.nodes.cfg.LamaSeqNode;

@NodeInfo(shortName = "seq ref")
public class LamaRefSeqNode extends LamaRefNode {

    @Child
    private LamaExprNode leftExpr;

    @Child
    private LamaRefNode rightExpr;

    public LamaRefSeqNode(LamaExprNode left, LamaRefNode right) {
        this.leftExpr = left;
        this.rightExpr = right;
    }


    public Object execute(VirtualFrame frame) {
        this.leftExpr.execute(frame);
        return this.rightExpr.execute(frame);
    }

}
