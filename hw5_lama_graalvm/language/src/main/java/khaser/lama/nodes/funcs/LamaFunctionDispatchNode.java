package khaser.lama.nodes.funcs;

import com.oracle.truffle.api.nodes.Node;
import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.CallTarget;

import khaser.lama.LamaContext;

public final class LamaFunctionDispatchNode extends Node {
    private String sym;

    public LamaFunctionDispatchNode(String sym) {
        this.sym = sym;
    }

    public CallTarget execute(VirtualFrame frame) {
        return LamaContext.get(this).globFunGet(this.sym);
    }
}
