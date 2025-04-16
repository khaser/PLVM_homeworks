package khaser.lama.nodes.cfg.pat_match;

import com.oracle.truffle.api.CompilerAsserts;
import com.oracle.truffle.api.CompilerDirectives;
import com.oracle.truffle.api.HostCompilerDirectives;
import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.ExplodeLoop;
import khaser.lama.nodes.LamaExprNode;
import khaser.lama.nodes.LamaScopeNode;

public class LamaCaseNode extends LamaExprNode {

    @Child
    private LamaExprNode scrut;
    public final LamaPattern[] patterns;
    @Children
    public LamaScopeNode[] branches;

    public LamaCaseNode(LamaExprNode scrut, LamaPattern[] patterns, LamaScopeNode[] branches) {
        this.scrut = scrut;
        this.patterns = patterns;
        this.branches = branches;
    }

    @Override
    @ExplodeLoop
    public Object execute(VirtualFrame frame) {
        var scrutVal = scrut.execute(frame);
        Object res = null;

        getContext().pushScope();

        CompilerAsserts.compilationConstant(branches.length);
        CompilerAsserts.compilationConstant(patterns.length);

        for (int i = 0; i < branches.length; ++i) {
            if (checkAndExecuteDefs(patterns[i], scrutVal)) {
                res = branches[i].execute(frame);
                break;
            }
        }

        getContext().popScope();
        return res;
    }

    @CompilerDirectives.TruffleBoundary
    private boolean checkAndExecuteDefs(LamaPattern pat, Object scrutVal) {
        if (pat.checkMatch(scrutVal)) {
            for (Binding bind : pat.collectBindings(scrutVal)) {
                getContext().defVar(bind.bindName, bind.value);
            }
            return true;
        } else {
            return false;
        }
    }
}
