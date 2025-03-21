package khaser.lama;

import com.oracle.truffle.api.CallTarget;
import com.oracle.truffle.api.CompilerDirectives;

import java.util.HashMap;
import java.util.Stack;

import static khaser.lama.LamaContext.unwrapRef;

public class LamaFrame {
    private final Stack<HashMap<String, Object[]>> vars = new Stack<>();
    private final Stack<HashMap<String, CallTarget>> funcs = new Stack<>();

    final private LamaFrame rootFrame;

    public LamaFrame(LamaFrame rootFrame) {
        this.rootFrame = rootFrame;
        pushScope();
    }

    public void pushScope() {
        vars.push(new HashMap<>());
        funcs.push(new HashMap<>());
    }

    public void popScope() {
        vars.pop();
        funcs.pop();
    }

    @CompilerDirectives.TruffleBoundary
    Object[] getVarRef(String sym) {
        return vars.stream().filter(scope -> scope.containsKey(sym)).reduce((a, b) -> b)
                            .map(m -> m.get(sym))
                            .orElseGet(() -> rootFrame.getVarRef(sym));
    }

    @CompilerDirectives.TruffleBoundary
    Object getVar(String sym) {
        return unwrapRef(getVarRef(sym));
    }

    @CompilerDirectives.TruffleBoundary
    CallTarget getFun(String sym) {
        return funcs.stream().filter(scope -> scope.containsKey(sym)).reduce((a, b) -> b)
                             .map(m -> m.get(sym))
                             .orElseGet(() -> rootFrame.getFun(sym));
    }

    @CompilerDirectives.TruffleBoundary
    public void defFun(String sym, CallTarget func) {
        funcs.peek().put(sym, func);
    }

    @CompilerDirectives.TruffleBoundary
    public void defVar(String sym, Object val) {
        vars.peek().put(sym, LamaContext.wrapRef(val));
    }
}
