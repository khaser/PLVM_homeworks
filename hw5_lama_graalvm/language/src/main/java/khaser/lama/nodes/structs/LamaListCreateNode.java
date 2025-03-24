package khaser.lama.nodes.structs;

import com.oracle.truffle.api.CompilerAsserts;
import com.oracle.truffle.api.CompilerDirectives;
import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.ExplodeLoop;
import khaser.lama.nodes.LamaExprNode;

import java.util.Arrays;

public class LamaListCreateNode extends LamaExprNode {
    @Children
    private final LamaExprNode[] els;

    public LamaListCreateNode(LamaExprNode[] els)  {
        this.els = els;
    }
    @Override
    @ExplodeLoop
    public LamaList execute(VirtualFrame frame) {
        CompilerAsserts.compilationConstant(this.els.length);
        Object[] vals = new Object[this.els.length];
        for (int i = 0; i < this.els.length; i++) {
            vals[i] = this.els[i].execute(frame);
        }
        return createListInInterp(vals);
    }

    @CompilerDirectives.TruffleBoundary
    public LamaList createListInInterp(Object[] vals) {
        return new LamaList(Arrays.stream(vals));
    }
}
