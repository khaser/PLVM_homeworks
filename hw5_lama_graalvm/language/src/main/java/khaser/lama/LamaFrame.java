package khaser.lama;

import com.oracle.truffle.api.CallTarget;
import com.oracle.truffle.api.CompilerDirectives;
import com.oracle.truffle.api.nodes.ExplodeLoop;

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
        for (int i = vars.size() - 1; i >= 0; --i) {
            if (vars.get(i).containsKey(sym)) {
                return vars.get(i).get(sym);
            }
        }
        return rootFrame.getVarRef(sym);
    }

    Object getVar(String sym) {
        return unwrapRef(getVarRef(sym));
    }

    CallTarget getFun(String sym) {
        for (int i = funcs.size() - 1; i >= 0; --i) {
            if (funcs.get(i).containsKey(sym)) {
                return funcs.get(i).get(sym);
            }
        }
        return rootFrame.getFun(sym);
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
