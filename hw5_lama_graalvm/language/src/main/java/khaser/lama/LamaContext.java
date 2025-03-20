package khaser.lama;

import com.oracle.truffle.api.CallTarget;
import com.oracle.truffle.api.TruffleLanguage;
import com.oracle.truffle.api.nodes.Node;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.Scanner;
import java.util.HashMap;
import java.util.Stack;

public final class LamaContext {

    private static final TruffleLanguage.ContextReference<LamaContext> REF =
          TruffleLanguage.ContextReference.create(LamaLanguage.class);
    private final Scanner inputScanner;
    public static LamaContext get(Node node) {
        return REF.get(node);
    }

    public static Object[] wrapRef(Object o) {
        return new Object[]{o};
    }

    public static Object unwrapRef(Object[] o) {
        return o[0];
    }

    private final LamaFrame globalFrame = new LamaFrame();
    final private Stack<LamaFrame> funFrames = new Stack<>();

    public LamaContext(TruffleLanguage.Env env) {
        this.inputScanner = new Scanner(new BufferedReader(new InputStreamReader(env.in())));
        this.globalFrame.pushScope();
    }

    public Scanner getInputScanner() {
        return this.inputScanner;
    }

    public void pushScope() {
        funFrames.peek().pushScope();
    }

    public void popScope() {
        funFrames.peek().popScope();
    }

    public void pushFrame() {
        funFrames.push(new LamaFrame());
    }

    public void popFrame() {
        funFrames.pop();
    }

    public Object getVar(String sym) {
        return unwrapRef(getVarRef(sym));
    }

    public Object[] getVarRef(String sym) {
        var scope = funFrames.peek().findVarScope(sym)
                    .or(() -> globalFrame.findVarScope(sym))
                    .orElseThrow();
        return scope.get(sym);
    }

    public CallTarget getFun(String sym) {
        var scope = this.funFrames.peek().findFuncScope(sym)
                    .or(() -> this.globalFrame.findFuncScope(sym))
                    .orElseThrow();
        return scope.get(sym);
    }

    public void defFun(String sym, CallTarget func) {
        funFrames.peek().defFun(sym, func);
    }

    public void defVar(String sym, Object value) {
        funFrames.peek().defVar(sym, value);
    }

    public void defFunGlobal(String sym, CallTarget func) {
        globalFrame.defFun(sym, func);
    }

    public void defVarGlobal(String sym, Object value) {
        globalFrame.defVar(sym, value);
    }
}
