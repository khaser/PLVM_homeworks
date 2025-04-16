package khaser.lama.nodes.refs;

import com.oracle.truffle.api.frame.VirtualFrame;

public class LamaRefVarNode extends LamaRefNode {

    private String sym;

    public LamaRefVarNode(String sym) {
        this.sym = sym;
    }

    @Override
    public Object[] execute(VirtualFrame frame) {
        return getContext().getVarRef(sym);
    }
}
