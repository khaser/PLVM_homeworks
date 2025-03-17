package khaser.lama.nodes.cfg;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.NodeInfo;

import khaser.lama.nodes.LamaExprNode;

@NodeInfo(shortName = "seq")
public class LamaSeqNode extends LamaExprNode {

    @Child
    private LamaExprNode leftExpr;

    @Child
    private LamaExprNode rightExpr;

    public LamaSeqNode(LamaExprNode left, LamaExprNode right) {
        this.leftExpr = left;
        this.rightExpr = right;
    }


    public Object execute(VirtualFrame frame) {
        this.leftExpr.execute(frame);
        return this.rightExpr.execute(frame);
    }

}
