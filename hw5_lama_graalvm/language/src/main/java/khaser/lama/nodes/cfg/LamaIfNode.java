package khaser.lama.nodes.cfg;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.NodeInfo;
import khaser.lama.nodes.LamaDefNode;
import khaser.lama.nodes.LamaExprNode;
import khaser.lama.nodes.LamaFunDefNode;
import khaser.lama.nodes.LamaScopeNode;

@NodeInfo(shortName = "if")
public class LamaIfNode extends LamaExprNode {

    @Child
    private LamaExprNode predicate;

    @Child
    private LamaScopeNode thenScope;

    @Child
    private LamaScopeNode elseScope;

    public LamaIfNode(LamaExprNode pred, LamaScopeNode thenScope) {
        this.predicate = pred;
        this.thenScope = thenScope;
        this.elseScope = new LamaScopeNode(new LamaDefNode[0], new LamaFunDefNode[0], new LamaSkipNode());
    }

    public LamaIfNode(LamaExprNode pred, LamaScopeNode thenScope, LamaScopeNode elseScope) {
        this.predicate = pred;
        this.thenScope = thenScope;
        this.elseScope = elseScope;
    }

    public Object execute(VirtualFrame frame) {
        Integer predVal = (Integer) this.predicate.execute(frame);
        if (predVal != 0) {
            return this.thenScope.execute(frame);
        } else {
            return this.elseScope.execute(frame);
        }
    }

}
