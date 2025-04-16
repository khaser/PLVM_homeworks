package khaser.lama.nodes.refs;

import com.oracle.truffle.api.dsl.NodeChild;
import com.oracle.truffle.api.dsl.Specialization;
import khaser.lama.nodes.LamaExprNode;

@NodeChild(value = "dest", type = LamaRefNode.class)
@NodeChild(value = "val", type = LamaExprNode.class)
public abstract class LamaAssignNode extends LamaExprNode {

    @Specialization
    public Object exec(Object[] ref, Object val) {
        ref[0] = val;
        return val;
    }

}
