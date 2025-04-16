package khaser.lama.nodes;

import com.oracle.truffle.api.CompilerDirectives;
import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.ExplodeLoop;
import com.oracle.truffle.api.nodes.NodeInfo;
import khaser.lama.nodes.cfg.LamaSeqNode;
import khaser.lama.nodes.cfg.LamaSkipNode;
import khaser.lama.nodes.refs.LamaAssignNodeGen;
import khaser.lama.nodes.refs.LamaRefVarNode;

import java.util.Arrays;

@NodeInfo(shortName = "scope")
public class LamaScopeNode extends LamaNode {

    @Children
    private final LamaDefNode[] defs;
    @Children
    private final LamaFunDefNode[] funDefs;

    @Child
    private LamaExprNode expr;

    public LamaScopeNode(LamaDefNode[] defs, LamaFunDefNode[] funDefs, LamaExprNode expr) {
        this.defs = defs;
        this.expr = expr;
        this.funDefs = funDefs;
    }

    @Override
    @ExplodeLoop
    public Object execute(VirtualFrame frame) {
        CompilerDirectives.ensureVirtualized(frame);
        getContext().pushScope();
        for (LamaDefNode def : this.defs) {
            def.execute(frame);
        }
        for (var funDef : this.funDefs) {
            funDef.execute(frame);
        }
        var res = this.expr.execute(frame);
        getContext().popScope();
        return res;
    }

    public LamaScopeNode concat(LamaExprNode tail) {
        return new LamaScopeNode(this.defs, this.funDefs, new LamaSeqNode(this.expr, tail));
    }

    private LamaScopeNode concatBefore(LamaExprNode head) {
        return new LamaScopeNode(this.defs, this.funDefs, new LamaSeqNode(head, this.expr));
    }

    public LamaScopeNode defsToAssigns() {
        var assigns = Arrays.stream(defs)
                .map(def -> (LamaExprNode) LamaAssignNodeGen.create(new LamaRefVarNode(def.getSym()), def.getExpr()))
                .reduce(LamaSeqNode::new)
                .orElse(new LamaSkipNode());
        return new LamaScopeNode(new LamaDefNode[0], this.funDefs, new LamaSeqNode(assigns, expr));
    }
}
