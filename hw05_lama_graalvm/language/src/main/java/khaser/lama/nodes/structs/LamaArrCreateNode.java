package khaser.lama.nodes.structs;

import com.oracle.truffle.api.CompilerAsserts;
import com.oracle.truffle.api.CompilerDirectives;
import com.oracle.truffle.api.dsl.NodeChildren;
import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.ExplodeLoop;
import com.oracle.truffle.api.nodes.Node;
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
    @ExplodeLoop
    public LamaArray execute(VirtualFrame frame) {
        CompilerAsserts.compilationConstant(this.els.length);
        Object[] vals = new Object[this.els.length];
        for (int i = 0; i < this.els.length; i++) {
            vals[i] = this.els[i].execute(frame);
        }
        return createArrayInInterp(vals);
    }

    @CompilerDirectives.TruffleBoundary
    public LamaArray createArrayInInterp(Object[] vals) {
        return new LamaArray(vals);
    }
}
