package khaser.lama.nodes;

import com.oracle.truffle.api.dsl.NodeChild;
import com.oracle.truffle.api.dsl.Specialization;
import com.oracle.truffle.api.nodes.NodeInfo;

@NodeInfo(shortName = "neg")
@NodeChild("expr")
public abstract class LamaNegNode extends LamaExprNode {
    @Specialization
    public Integer execInt(int expr) {
        return -expr;
    }
}