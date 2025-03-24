package khaser.lama.nodes.structs;

import com.oracle.truffle.api.CompilerDirectives;
import com.oracle.truffle.api.dsl.NodeChild;
import com.oracle.truffle.api.dsl.Specialization;
import com.oracle.truffle.api.frame.VirtualFrame;
import khaser.lama.nodes.LamaExprNode;

import java.util.Arrays;

@NodeChild("head")
@NodeChild("tail")
public abstract class LamaListConsNode extends LamaExprNode {

    @Specialization
    public LamaList exec(Object head, LamaList tail) {
        return tail.cons(head);
    }
}
