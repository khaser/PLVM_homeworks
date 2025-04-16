package khaser.lama.nodes.refs;

import com.oracle.truffle.api.dsl.NodeChild;
import com.oracle.truffle.api.dsl.Specialization;
import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.Node;
import com.oracle.truffle.api.nodes.NodeInfo;
import khaser.lama.nodes.LamaDefNode;
import khaser.lama.nodes.LamaExprNode;
import khaser.lama.nodes.LamaFunDefNode;
import khaser.lama.nodes.LamaScopeNode;
import khaser.lama.nodes.cfg.LamaSkipNode;

@NodeChild(value="predicate", type=LamaExprNode.class)
@NodeChild("thenRef")
@NodeChild("elseRef")
public abstract class LamaRefIfNode extends LamaRefNode {

    @Specialization
    public Object[] exec(int predicate, Object[] thenRef, Object[] elseRef) {
        if (predicate != 0) {
            return thenRef;
        } else {
            return elseRef;
        }
    }

}
