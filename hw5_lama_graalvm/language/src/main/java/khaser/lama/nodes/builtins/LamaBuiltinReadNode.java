package khaser.lama.nodes.builtins;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.NodeInfo;

import khaser.lama.nodes.LamaNode;

@NodeInfo(shortName = "read")
public class LamaBuiltinReadNode extends LamaNode {

    public LamaBuiltinReadNode() {
    }

    @Override
    public Integer execute(VirtualFrame frame) {
        System.out.print("> ");
        return getContext().getInputScaner().nextInt();
    }

}
