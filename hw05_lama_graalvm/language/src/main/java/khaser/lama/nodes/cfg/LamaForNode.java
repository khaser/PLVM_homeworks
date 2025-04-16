package khaser.lama.nodes.cfg;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.NodeInfo;
import khaser.lama.nodes.LamaExprNode;
import khaser.lama.nodes.LamaScopeNode;

@NodeInfo(shortName = "for")
public class LamaForNode extends LamaExprNode {

    @Child
    private LamaScopeNode loop;

    public LamaForNode(LamaScopeNode init, LamaExprNode cond, LamaExprNode iter, LamaScopeNode body) {
        this.loop = init.concat(new LamaWhileDoNode(cond, body.concat(iter)));
    }
    public Integer execute(VirtualFrame frame) {
        this.loop.execute(frame);
        return 0;
    }

}
