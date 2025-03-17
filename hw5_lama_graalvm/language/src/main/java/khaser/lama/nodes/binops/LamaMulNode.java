package khaser.lama.nodes.binops;

import com.oracle.truffle.api.dsl.NodeChild;
import com.oracle.truffle.api.dsl.Specialization;
import com.oracle.truffle.api.nodes.NodeInfo;
import khaser.lama.nodes.LamaExprNode;

@NodeInfo(shortName = "mul")
@NodeChild("left")
@NodeChild("right")
public abstract class LamaMulNode extends LamaExprNode {
    @Specialization
    public Integer execInt(int left, int right) {
        return left * right;
    }

}
