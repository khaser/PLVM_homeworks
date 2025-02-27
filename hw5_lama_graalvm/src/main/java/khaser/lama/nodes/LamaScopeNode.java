package khaser.lama.nodes;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.NodeInfo;

@NodeInfo(shortName = "scope")
public class LamaScopeNode extends LamaNode {

    @Children
    private final LamaDefNode[] defs;

    @Child
    private LamaExprNode expr;

    public LamaScopeNode(LamaDefNode[] defs, LamaExprNode expr) {
        this.defs = defs;
        this.expr = expr;
    }

    @Override
    public Integer execute(VirtualFrame frame) {
        for (LamaDefNode def : this.defs) {
            def.execute(frame);
        }
        return this.expr.execute(frame);
    }

}
