package khaser.lama.nodes;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.NodeInfo;

@NodeInfo(shortName = "nested scope")
public class LamaNestedScope extends LamaExprNode {
    @Child
    private LamaScopeNode scope;

    public LamaNestedScope(LamaScopeNode scope) {
        this.scope = scope;
    }

    @Override
    public Object execute(VirtualFrame frame) {
        return this.scope.execute(frame);
    }
}
