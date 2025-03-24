package khaser.lama.nodes;

import com.oracle.truffle.api.frame.VirtualFrame;


public class LamaDefNode extends LamaNode {

    private final String sym;

    @Child
    private LamaExprNode expr;

    public LamaDefNode(String sym, LamaExprNode expr) {
        this.sym = sym;
        this.expr = expr;
    }

    @Override
    public Object execute(VirtualFrame frame) {
        Object exprVal = this.expr.execute(frame);
        getContext().defVar(sym, exprVal);
        return exprVal;
    }

    public LamaExprNode getExpr() {
        return expr;
    }

    public String getSym() {
        return sym;
    }
}
