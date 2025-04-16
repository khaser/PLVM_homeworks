package khaser.lama.nodes;

import com.oracle.truffle.api.frame.VirtualFrame;

import khaser.lama.nodes.LamaNode;

public class LamaReadNode extends LamaExprNode {

    private String sym;

    public LamaReadNode(String sym) {
        this.sym = sym;
    }

    @Override
    public Object execute(VirtualFrame frame) {
        return getContext().getVar(sym);
    }
}
