package khaser.lama.nodes.binops;

import com.oracle.truffle.api.dsl.NodeChild;
import com.oracle.truffle.api.dsl.Specialization;
import com.oracle.truffle.api.nodes.NodeInfo;
import khaser.lama.nodes.LamaExprNode;

@NodeInfo(shortName = "land")
@NodeChild("left")
@NodeChild("right")
public abstract class LamaLandNode extends LamaExprNode {
    @Specialization
    public Integer execInt(int left, int right) {
        return left != 0 && right != 0 ? 1 : 0;
    }

}
