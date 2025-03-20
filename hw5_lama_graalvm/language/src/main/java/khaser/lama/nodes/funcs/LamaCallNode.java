package khaser.lama.nodes.funcs;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.NodeInfo;
import com.oracle.truffle.api.CallTarget;

import khaser.lama.nodes.LamaExprNode;

@NodeInfo(shortName = "function call")
public final class LamaCallNode extends LamaExprNode {

    private LamaFunctionDispatchNode target;

    @Children
    private final LamaExprNode[] callArgs;

    public LamaCallNode(LamaFunctionDispatchNode target, LamaExprNode[] callArgs) {
        this.target = target;
        this.callArgs = callArgs;
    }

    @Override
    public Object execute(VirtualFrame frame) {
        CallTarget callTarget = this.target.execute(frame);

        Object[] argVals = new Object[this.callArgs.length];
        for (int i = 0; i < this.callArgs.length; i++) {
            argVals[i] = this.callArgs[i].execute(frame);
        }
        getContext().pushFrame();
        var res = callTarget.call(argVals);
        getContext().popFrame();
        return res;
    }

}
