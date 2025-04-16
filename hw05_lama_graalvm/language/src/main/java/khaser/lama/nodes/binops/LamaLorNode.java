package khaser.lama.nodes.binops;

import com.oracle.truffle.api.dsl.NodeChild;
import com.oracle.truffle.api.dsl.Specialization;
import com.oracle.truffle.api.nodes.NodeInfo;
import khaser.lama.nodes.LamaExprNode;

@NodeInfo(shortName = "lor")
@NodeChild("left")
@NodeChild("right")
public abstract class LamaLorNode extends LamaExprNode {
    @Specialization
    public Integer execInt(int left, int right) {
        return left != 0 || right != 0 ? 1 : 0;
    }

}
