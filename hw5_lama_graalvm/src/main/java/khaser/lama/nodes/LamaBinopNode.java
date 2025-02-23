package khaser.lama.nodes;

import com.oracle.truffle.api.dsl.NodeChild;
import com.oracle.truffle.api.dsl.Specialization;

public abstract class LamaBinopNode extends LamaExprNode {

    @SuppressWarnings("FieldMayBeFinal")
    @Child
    protected LamaExprNode leftNode, rightNode;

    protected LamaBinopNode(LamaExprNode leftNode, LamaExprNode rightNode) {
        this.leftNode = leftNode;
        this.rightNode = rightNode;
    }

}
