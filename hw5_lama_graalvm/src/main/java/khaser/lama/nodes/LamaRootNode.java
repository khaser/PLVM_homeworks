package khaser.lama.nodes;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.RootNode;

public final class LamaRootNode extends RootNode {
    @SuppressWarnings("FieldMayBeFinal")
    @Child
    private LamaExprNode exprNode;

    public LamaRootNode(LamaExprNode exprNode) {
        super(null);

        this.exprNode = exprNode;
    }

    @Override
    public Object execute(VirtualFrame frame) {
        return this.exprNode.execInt(frame);
    }
}
