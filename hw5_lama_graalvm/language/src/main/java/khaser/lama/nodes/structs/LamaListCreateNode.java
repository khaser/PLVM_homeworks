package khaser.lama.nodes.structs;

import com.oracle.truffle.api.frame.VirtualFrame;
import khaser.lama.nodes.LamaExprNode;

import java.util.Arrays;

public class LamaListCreateNode extends LamaExprNode {
    @Children
    private final LamaExprNode[] els;

    public LamaListCreateNode(LamaExprNode[] els)  {
        this.els = els;
    }
    @Override
    public LamaList execute(VirtualFrame frame) {
        return new LamaList(Arrays.stream(this.els).map(expr -> expr.execute(frame)));
    }
}
