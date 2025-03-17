package khaser.lama.nodes.structs;

import com.oracle.truffle.api.dsl.NodeChild;
import com.oracle.truffle.api.dsl.Specialization;
import khaser.lama.nodes.LamaExprNode;

@NodeChild("arr")
@NodeChild("idx")
public abstract class LamaArrReadNode extends LamaExprNode {
    @Specialization
    public Object execArray(Object[] array, int idx) {
        return array[idx];
    }
}