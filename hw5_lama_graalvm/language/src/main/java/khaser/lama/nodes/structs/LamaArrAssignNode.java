package khaser.lama.nodes.structs;

import com.oracle.truffle.api.dsl.NodeChild;
import com.oracle.truffle.api.dsl.Specialization;
import khaser.lama.nodes.LamaExprNode;

@NodeChild("idx")
@NodeChild("val")
public abstract class LamaArrAssignNode extends LamaExprNode {
    String lit;

    LamaArrAssignNode(String lit) {
        this.lit = lit;
    }

    @Specialization
    public Object execArray(int idx, Object val) {
        Object[] arr = (Object[]) getContext().getVar(this.lit);
        return arr[idx] = val;
    }
}