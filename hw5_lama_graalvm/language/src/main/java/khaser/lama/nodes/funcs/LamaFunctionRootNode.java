package khaser.lama.nodes.funcs;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.RootNode;

import khaser.lama.LamaLanguage;
import khaser.lama.nodes.LamaNode;
import khaser.lama.nodes.LamaScopeNode;

public final class LamaFunctionRootNode extends RootNode {
    @SuppressWarnings("FieldMayBeFinal")
    @Child
    private LamaNode entry;

    public LamaFunctionRootNode(LamaLanguage lang, LamaNode entry) {
        super(lang);
        this.entry = entry;
    }

    @Override
    public Integer execute(VirtualFrame frame) {
        return this.entry.execute(frame);
    }
}
