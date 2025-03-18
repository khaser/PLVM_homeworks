package khaser.lama.nodes.structs;

import com.oracle.truffle.api.frame.VirtualFrame;
import khaser.lama.LamaContext;
import khaser.lama.nodes.LamaExprNode;

public class LamaStringLiteralNode extends LamaExprNode {

    String str;

    public LamaStringLiteralNode(String str)  {
        this.str = str;
    }
    @Override
    public Object[] execute(VirtualFrame frame) {
        return str.chars().boxed().map(LamaContext::wrapRef).toArray();
    }
}
