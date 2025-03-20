package khaser.lama.nodes;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.NodeInfo;
import khaser.lama.nodes.cfg.LamaSeqNode;

@NodeInfo(shortName = "scope")
public class LamaScopeNode extends LamaNode {

    @Children
    private final LamaDefNode[] defs;
    private final LamaFunDefNode[] funDefs;

    @Child
    private LamaExprNode expr;

    public LamaScopeNode(LamaDefNode[] defs, LamaFunDefNode[] funDefs, LamaExprNode expr) {
        this.defs = defs;
        this.expr = expr;
        this.funDefs = funDefs;
    }

    @Override
    public Object execute(VirtualFrame frame) {
        getContext().pushScope();
        for (LamaDefNode def : this.defs) {
            def.execute(frame);
        }
        for (var funDef : this.funDefs) {
            funDef.register();
        }
        var res = this.expr.execute(frame);
        getContext().popScope();
        return res;
    }

    public Object executeGlobal(VirtualFrame frame) {
        getContext().pushFrame();
        for (LamaDefNode def : this.defs) {
            def.executeGlobal(frame);
        }
        for (var funDef : this.funDefs) {
            funDef.registerGlobal();
        }
        var res = this.expr.execute(frame);
        getContext().popFrame();
        return res;
    }

    public LamaScopeNode concat(LamaExprNode tail) {
        return new LamaScopeNode(this.defs, this.funDefs, new LamaSeqNode(this.expr, tail));
    }

}
