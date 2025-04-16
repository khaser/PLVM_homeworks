package khaser.lama.nodes.refs;

import com.oracle.truffle.api.dsl.NodeChild;
import com.oracle.truffle.api.dsl.Specialization;
import khaser.lama.LamaContext;
import khaser.lama.nodes.LamaExprNode;
import khaser.lama.nodes.structs.LamaArray;

@NodeChild(value = "arr", type = LamaWeakNode.class)
@NodeChild(value = "idx", type = LamaExprNode.class)
public abstract class LamaWeakArrNode extends LamaWeakNode {
    @Specialization
    public Object execArray(Object[] arrayRef, int idx) {
        LamaArray array = (LamaArray) LamaContext.unwrapRef(arrayRef);
        return array.getRef(idx);
    }
}