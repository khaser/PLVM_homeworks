package khaser.lama.nodes.structs;

import com.oracle.truffle.api.dsl.NodeChild;
import com.oracle.truffle.api.dsl.Specialization;
import khaser.lama.LamaContext;
import khaser.lama.nodes.LamaExprNode;

@NodeChild("arr")
@NodeChild(value = "idx")
public abstract class LamaArrIdxNode extends LamaExprNode {

    @Specialization
    public Object execArray(LamaArray array, int idx) {
        return array.getEl(idx);
    }

}