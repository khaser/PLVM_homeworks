package khaser.lama.nodes.cfg.pat_match;

import com.oracle.truffle.api.frame.VirtualFrame;
import khaser.lama.nodes.LamaExprNode;

public class LamaCaseNode extends LamaExprNode {

    private LamaExprNode scrut;
    private LamaCaseBranch[] branches;

    public LamaCaseNode(LamaExprNode scrut, LamaCaseBranch[] branches) {
        this.scrut = scrut;
        this.branches = branches;
    }

    @Override
    public Object execute(VirtualFrame frame) {
        var scrutVal = scrut.execute(frame);
        for (LamaCaseBranch branch : branches) {
            if (branch.pat.checkMatch(scrutVal)) {
                return executeBranch(frame, branch, scrutVal);
            }
        }
        return 0;
    }

    private Object executeBranch(VirtualFrame frame, LamaCaseBranch branch, Object scrutVal) {
        var ctx = getContext();
        ctx.pushScope();
        for (Binding bind : branch.pat.collectBindings(scrutVal)) {
            ctx.defVar(bind.bindName, bind.value);
        }
        var res = branch.scope.execute(frame);
        ctx.popScope();
        return res;
    }
}
