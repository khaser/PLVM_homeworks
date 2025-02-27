package khaser.lama.nodes.cfg;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.NodeInfo;

import khaser.lama.nodes.LamaExprNode;

@NodeInfo(shortName = "skip")
public class LamaSkipNode extends LamaExprNode {

    public LamaSkipNode() {
    }

    public Integer execute(VirtualFrame frame) {
        return 0;
    }

}
