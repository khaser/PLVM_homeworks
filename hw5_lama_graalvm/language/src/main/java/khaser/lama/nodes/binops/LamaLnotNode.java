package khaser.lama.nodes.binops;

import com.oracle.truffle.api.dsl.NodeChild;
import com.oracle.truffle.api.dsl.Specialization;
import com.oracle.truffle.api.nodes.NodeInfo;
import khaser.lama.nodes.LamaExprNode;

@NodeInfo(shortName = "lnot")
@NodeChild("expr")
public abstract class LamaLnotNode extends LamaExprNode {
    @Specialization
    public Integer execInt(int expr) {
        return expr != 0 ? 0 : 1;
    }

}
