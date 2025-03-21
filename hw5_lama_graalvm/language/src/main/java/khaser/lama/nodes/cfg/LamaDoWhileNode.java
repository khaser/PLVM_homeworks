package khaser.lama.nodes.cfg;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.NodeInfo;
import khaser.lama.nodes.LamaExprNode;
import khaser.lama.nodes.LamaScopeNode;

@NodeInfo(shortName = "do while")
public class LamaDoWhileNode extends LamaExprNode {

    @Child
    private LamaScopeNode doWhile;

    public LamaDoWhileNode(LamaExprNode pred, LamaScopeNode body) {
        doWhile = body.concat(new LamaWhileDoNode(pred, body.defsToAssigns()));
    }


    public Integer execute(VirtualFrame frame) {
        this.doWhile.execute(frame);
        return 0;
    }

}
