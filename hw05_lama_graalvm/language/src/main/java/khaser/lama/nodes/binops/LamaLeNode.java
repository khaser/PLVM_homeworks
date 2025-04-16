package khaser.lama.nodes.binops;

import com.oracle.truffle.api.dsl.NodeChild;
import com.oracle.truffle.api.dsl.Specialization;
import com.oracle.truffle.api.nodes.NodeInfo;
import khaser.lama.nodes.LamaExprNode;

@NodeInfo(shortName = "le")
@NodeChild("left")
@NodeChild("right")
public abstract class LamaLeNode extends LamaExprNode {
    @Specialization
    public Integer execInt(int left, int right) {
        return left < right ? 1 : 0;
    }

}
