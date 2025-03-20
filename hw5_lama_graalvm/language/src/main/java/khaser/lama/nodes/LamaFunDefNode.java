package khaser.lama.nodes;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.Node;
import khaser.lama.nodes.builtins.LamaBuiltinWriteNode;
import khaser.lama.nodes.funcs.LamaFunctionRootNode;
import khaser.lama.nodes.funcs.LamaReadArgNode;

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
        register();
        return 0;
    }

    public void register() {
        var funRootNode = new LamaFunctionRootNode(currentTruffleLanguage(), this.body);
        getContext().defFun(funName, funRootNode.getCallTarget());
    }

    public void registerGlobal() {
        var funRootNode = new LamaFunctionRootNode(currentTruffleLanguage(), this.body);
        getContext().defFunGlobal(funName, funRootNode.getCallTarget());
    }
}
