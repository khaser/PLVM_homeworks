package khaser.lama.nodes.refs;

import com.oracle.truffle.api.frame.VirtualFrame;
import khaser.lama.nodes.LamaExprNode;

public class LamaWeakVarNode extends LamaWeakNode {

    private String sym;

    public LamaWeakVarNode(String sym) {
        this.sym = sym;
    }

    @Override
    public Object execute(VirtualFrame frame) {
        return getContext().getVarRef(sym);
    }
}
