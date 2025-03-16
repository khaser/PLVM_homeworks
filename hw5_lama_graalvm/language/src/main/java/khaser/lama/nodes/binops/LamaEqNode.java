package khaser.lama.nodes.binops;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.NodeInfo;
import khaser.lama.nodes.LamaBinopNode;
import khaser.lama.nodes.LamaExprNode;

@NodeInfo(shortName = "eq")
public class LamaEqNode extends LamaBinopNode {
    public LamaEqNode(LamaExprNode leftNode, LamaExprNode rightNode) {
        super(leftNode, rightNode);
    }

    public Integer execute(VirtualFrame frame) {
        Integer leftValue = this.leftNode.execute(frame);
        Integer rightValue = this.rightNode.execute(frame);
        return leftValue == rightValue ? 1 : 0;
    }

}
