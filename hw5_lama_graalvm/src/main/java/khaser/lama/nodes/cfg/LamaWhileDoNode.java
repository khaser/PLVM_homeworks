package khaser.lama.nodes.cfg;

import com.oracle.truffle.api.Truffle;
import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.LoopNode;
import com.oracle.truffle.api.nodes.NodeInfo;
import khaser.lama.nodes.LamaExprNode;
import khaser.lama.nodes.LamaScopeNode;

@NodeInfo(shortName = "while do")
public class LamaWhileDoNode extends LamaExprNode {

    @Child private LoopNode loop;

    public LamaWhileDoNode(LamaExprNode pred, LamaScopeNode body) {
        this.loop = Truffle.getRuntime().createLoopNode(new WhileInternalNode(pred, body));
    }
    public Integer execute(VirtualFrame frame) {
        this.loop.execute(frame);
        return 0;
    }

}