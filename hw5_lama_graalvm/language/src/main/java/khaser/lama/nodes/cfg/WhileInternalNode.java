package khaser.lama.nodes.cfg;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.Node;
import com.oracle.truffle.api.nodes.RepeatingNode;
import khaser.lama.nodes.LamaExprNode;
import khaser.lama.nodes.LamaScopeNode;

final class WhileInternalNode extends Node implements RepeatingNode {
    @Child
    private LamaExprNode predicate;
    @Child
    private LamaScopeNode body;

    public WhileInternalNode(LamaExprNode pred, LamaScopeNode body) {
        this.predicate = pred;
        this.body = body;
    }

    @Override
    public boolean executeRepeating(VirtualFrame frame) {
        Integer predVal = (Integer) this.predicate.execute(frame);
        if (predVal == 0) {
            return false;
        }
        this.body.execute(frame);
        return true;
    }
}
