package khaser.lama;

import com.oracle.truffle.api.CallTarget;
import com.oracle.truffle.api.TruffleLanguage;

import khaser.lama.LamaLanguage;
import khaser.lama.nodes.LamaNode;

import java.util.HashMap;

public final class LamaContext {

    private static final TruffleLanguage.ContextReference<LamaContext> REF =
          TruffleLanguage.ContextReference.create(LamaLanguage.class);

    public static LamaContext get(LamaNode node) {
        return REF.get(node);
    }

    private final HashMap<String, Integer> globs = new HashMap();


    public LamaContext() {
    }

    public void globSet(String sym, int value) {
        this.globs.put(sym, value);
    }

    public int globGet(String sym) {
        return this.globs.get(sym);
    }

}
