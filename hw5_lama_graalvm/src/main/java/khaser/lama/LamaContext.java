package khaser.lama;

import com.oracle.truffle.api.CallTarget;
import com.oracle.truffle.api.TruffleLanguage;
import com.oracle.truffle.api.nodes.Node;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.HashMap;

public final class LamaContext {

    private static final TruffleLanguage.ContextReference<LamaContext> REF =
          TruffleLanguage.ContextReference.create(LamaLanguage.class);
    private final BufferedReader input;
    public static LamaContext get(Node node) {
        return REF.get(node);
    }

    private final HashMap<String, Integer> globs = new HashMap();
    private final HashMap<String, CallTarget> globFuns = new HashMap();


    public LamaContext(TruffleLanguage.Env env) {
        this.input = new BufferedReader(new InputStreamReader(env.in()));
    }

    public BufferedReader getInput() {
        return input;
    }
    public void globSet(String sym, int value) {
        this.globs.put(sym, value);
    }

    public int globGet(String sym) {
        return this.globs.get(sym);
    }

    public void globFunSet(String sym, CallTarget value) {
        this.globFuns.put(sym, value);
    }

    public CallTarget globFunGet(String sym) {
        return this.globFuns.get(sym);
    }

}
