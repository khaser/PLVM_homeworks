package khaser.lama.nodes.binops;

import com.oracle.truffle.api.dsl.NodeChild;
import com.oracle.truffle.api.dsl.Specialization;
import khaser.lama.nodes.LamaExprNode;

public abstract class LamaBinopNode extends LamaExprNode {

    @SuppressWarnings("FieldMayBeFinal")
    @Child
    protected LamaExprNode leftNode, rightNode;

    protected LamaBinopNode(LamaExprNode leftNode, LamaExprNode rightNode) {
        this.leftNode = leftNode;
        this.rightNode = rightNode;
    }

}
