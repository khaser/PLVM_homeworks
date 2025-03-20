package khaser.lama.nodes;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.RootNode;

public final class LamaRootNode extends RootNode {
    @SuppressWarnings("FieldMayBeFinal")
    @Child
    private LamaScopeNode root;

    public LamaRootNode(LamaScopeNode root) {
        super(null);

        this.root = root;
    }

    @Override
    public Object execute(VirtualFrame frame) {
        return this.root.executeGlobal(frame);
    }
}
