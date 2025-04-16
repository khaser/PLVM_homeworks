package khaser.lama.nodes.structs;

import com.oracle.truffle.api.CompilerAsserts;
import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.Node;
import khaser.lama.LamaContext;
import khaser.lama.nodes.LamaExprNode;

import java.util.Arrays;

public class LamaSexprCreateNode extends LamaExprNode {

    String ident;
    @Node.Children
    LamaExprNode[] args;

    public LamaSexprCreateNode(String ident, LamaExprNode[] args) {
        this.ident = ident;
        this.args = args;
    }

    @Override
    public Object execute(VirtualFrame frame) {
        CompilerAsserts.compilationConstant(args.length);
        Object[] vals = new Object[args.length];
        for (int i = 0; i < args.length; i++) {
            vals[i] = args[i].execute(frame);
        }
        return new LamaSexpr(ident, vals);
    }
}
