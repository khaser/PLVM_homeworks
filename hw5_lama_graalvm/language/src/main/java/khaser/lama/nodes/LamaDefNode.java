package khaser.lama.nodes;

import com.oracle.truffle.api.frame.VirtualFrame;

import com.oracle.truffle.api.dsl.NodeField;
import com.oracle.truffle.api.dsl.NodeChild;
import com.oracle.truffle.api.dsl.Specialization;

import khaser.lama.nodes.LamaNode;

public class LamaDefNode extends LamaNode {

    private String sym;

    @Child
    private LamaExprNode expr;

    public LamaDefNode(String sym, LamaExprNode expr) {
        this.sym = sym;
        this.expr = expr;
    }

    @Override
    public Integer execute(VirtualFrame frame) {
        Integer exprVal = this.expr.execute(frame);
        getContext().defVar(sym, exprVal);
        return exprVal;
    }
}
