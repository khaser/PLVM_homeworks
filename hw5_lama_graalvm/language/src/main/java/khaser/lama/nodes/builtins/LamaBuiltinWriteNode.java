package khaser.lama.nodes.builtins;

import com.oracle.truffle.api.CallTarget;
import com.oracle.truffle.api.CompilerDirectives;
import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.NodeInfo;

import java.util.Scanner;

import khaser.lama.nodes.LamaExprNode;
import khaser.lama.nodes.LamaNode;
import khaser.lama.nodes.funcs.LamaFunctionDispatchNode;

@NodeInfo(shortName = "write")
public class LamaBuiltinWriteNode extends LamaNode {

    @Child
    private LamaExprNode expr;

    public LamaBuiltinWriteNode(LamaExprNode expr) {
        this.expr = expr;
    }

    @Override
    public Integer execute(VirtualFrame frame) {
        Object val  = expr.execute(frame);
        print(val);
        return 0;
    }
    @CompilerDirectives.TruffleBoundary
    private void print(Object val) {
        System.out.println(val);
    }

}
