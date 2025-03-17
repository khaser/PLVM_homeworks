package khaser.lama.nodes.structs;

import com.oracle.truffle.api.dsl.NodeChild;
import com.oracle.truffle.api.dsl.Specialization;
import khaser.lama.nodes.LamaExprNode;

@NodeChild("arr")
@NodeChild("idx")
public abstract class LamaArrRead extends LamaExprNode {
    @Specialization
    public Object execArray(Object[] array, int idx) {
        return array[idx];
    }
}

//public class LamaArrRead extends LamaExprNode {
//    @Child
//    private LamaExprNode expr;
//
//    public LamaArrRead(LamaExprNode expr)  {
//        this.expr = expr;
//    }
//    @Override
//    public Object execute(VirtualFrame frame) {
//        Object[] arr = (Object[]) expr.execute(frame);
//
//        // TODO: get array from context
//        return 0;
//    }
//}
