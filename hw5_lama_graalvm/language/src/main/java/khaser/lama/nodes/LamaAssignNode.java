package khaser.lama.nodes;

import com.oracle.truffle.api.frame.VirtualFrame;

public class LamaAssignNode extends LamaExprNode {

    private String sym;

    @Child
    private LamaExprNode val;

    public LamaAssignNode(String sym, LamaExprNode val) {
        this.sym = sym;
        this.val = val;
    }

    @Override
    public Object execute(VirtualFrame frame) {
        Object res = val.execute(frame);
        getContext().setVar(sym, res);
        return res;
    }
}
