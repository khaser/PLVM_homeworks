package khaser.lama.nodes;

import com.oracle.truffle.api.CompilerDirectives;
import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.Node;
import khaser.lama.LamaFrame;
import khaser.lama.nodes.funcs.LamaFunctionRootNode;

public class LamaFunDefNode extends LamaNode {

    private String funName;

    @Node.Child
    private LamaScopeNode body;

    public LamaFunDefNode(String funName, LamaScopeNode body) {
        this.funName = funName;
        this.body = body;
    }

    @Override
    public Integer execute(VirtualFrame frame) {
        inner();
        return 0;
    }

    @CompilerDirectives.TruffleBoundary
    private void inner() {
        var ctx = getContext();
        var funRootNode = new LamaFunctionRootNode(currentTruffleLanguage(), this.body, new LamaFrame(ctx.curFrame), ctx);
        ctx.defFun(funName, funRootNode.getCallTarget());
    }

}
