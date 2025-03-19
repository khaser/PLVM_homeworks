package khaser.lama.nodes.builtins;

import com.oracle.truffle.api.dsl.NodeChild;
import com.oracle.truffle.api.dsl.Specialization;
import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.NodeInfo;
import khaser.lama.LamaContext;
import khaser.lama.nodes.LamaExprNode;
import khaser.lama.nodes.LamaNode;
import khaser.lama.nodes.structs.LamaSexpr;
import khaser.lama.nodes.structs.LamaString;

import java.util.Arrays;

@NodeInfo(shortName = "string builtin")
@NodeChild(value="expr", type=LamaExprNode.class)
public abstract class LamaBuiltinStringNode extends LamaNode {

    @Specialization
    public LamaString exec(Object expr) {
        return new LamaString(expr.toString());
    }
}
