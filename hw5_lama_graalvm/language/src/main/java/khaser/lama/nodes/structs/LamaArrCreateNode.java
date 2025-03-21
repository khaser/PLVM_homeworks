package khaser.lama.nodes.structs;

import com.oracle.truffle.api.frame.VirtualFrame;
import khaser.lama.LamaContext;
import khaser.lama.nodes.LamaExprNode;

import java.util.Arrays;

public class LamaArrCreateNode extends LamaExprNode {
    @Children
    private final LamaExprNode[] els;

    public LamaArrCreateNode(LamaExprNode[] els)  {
        this.els = els;
    }
    @Override
    public LamaArray execute(VirtualFrame frame) {
        return new LamaArray(Arrays.stream(this.els).map(expr -> expr.execute(frame)).toArray());
    }
}
