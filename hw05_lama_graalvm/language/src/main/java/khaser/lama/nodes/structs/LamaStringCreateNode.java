package khaser.lama.nodes.structs;

import com.oracle.truffle.api.frame.VirtualFrame;
import khaser.lama.LamaContext;
import khaser.lama.nodes.LamaExprNode;

public class LamaStringCreateNode extends LamaExprNode {

    String str;

    public LamaStringCreateNode(String str)  {
        this.str = str;
    }
    @Override
    public LamaString execute(VirtualFrame frame) {
        return new LamaString(str);
    }
}
