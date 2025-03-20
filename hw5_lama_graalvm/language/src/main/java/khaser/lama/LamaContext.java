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

    public LamaFrame curFrame;

    public static LamaContext get(Node node) {
        return REF.get(node);
    }

    public static Object[] wrapRef(Object o) {
        return new Object[]{o};
    }

    public static Object unwrapRef(Object[] o) {
        return o[0];
    }

    public LamaContext(TruffleLanguage.Env env) {
        this.inputScanner = new Scanner(new BufferedReader(new InputStreamReader(env.in())));
    }
    public Scanner getInputScanner() {
        return this.inputScanner;
    }

    public void pushScope() {
        curFrame.pushScope();
    }

    public void popScope() {
        curFrame.popScope();
    }

    public void defFun(String sym, CallTarget func) {
       curFrame.defFun(sym, func);
    }

    public void defVar(String sym, Object val) {
        curFrame.defVar(sym, val);
    }

    public Object[] getVarRef(String sym) {
        return curFrame.getVarRef(sym);
    }

    public Object getVar(String sym) {
        return curFrame.getVar(sym);
    }

    public CallTarget getFun(String sym) {
        return curFrame.getFun(sym);
    }
}
