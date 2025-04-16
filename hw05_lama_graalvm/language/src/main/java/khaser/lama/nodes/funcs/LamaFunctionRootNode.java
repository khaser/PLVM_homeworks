package khaser.lama.nodes.funcs;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.RootNode;

import khaser.lama.LamaContext;
import khaser.lama.LamaFrame;
import khaser.lama.LamaLanguage;
import khaser.lama.nodes.LamaNode;
import khaser.lama.nodes.LamaScopeNode;

public final class LamaFunctionRootNode extends RootNode {
    @SuppressWarnings("FieldMayBeFinal")
    @Child
    private LamaNode entry;

    private LamaFrame frame;

    private final LamaContext ctx;

    public LamaFunctionRootNode(LamaLanguage lang, LamaNode entry, LamaFrame frame, LamaContext ctx) {
        super(lang);
        this.entry = entry;
        this.frame = frame;
        this.ctx = ctx;
    }

    @Override
    public Object execute(VirtualFrame vframe) {
        var callerFrame = ctx.curFrame;
        ctx.curFrame = frame;
        var res = this.entry.execute(vframe);
        ctx.curFrame = callerFrame;
        return res;
    }
}
