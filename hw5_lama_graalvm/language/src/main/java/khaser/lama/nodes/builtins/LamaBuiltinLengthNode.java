package khaser.lama.nodes.builtins;

import com.oracle.truffle.api.dsl.NodeChild;
import com.oracle.truffle.api.dsl.Specialization;
import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.NodeInfo;
import khaser.lama.nodes.LamaNode;
import khaser.lama.nodes.structs.LamaArray;

@NodeInfo(shortName = "length")
@NodeChild("expr")
public abstract class LamaBuiltinLengthNode extends LamaNode {

    @Specialization
    public Integer execArray(LamaArray expr) {
        return expr.length();
    }

}
