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

    private final Stack<HashMap<String, Object>> varScopes = new Stack<>();
    private final Stack<HashMap<String, CallTarget>> funcScopes = new Stack<>();


    public LamaContext(TruffleLanguage.Env env) {
        this.inputScanner = new Scanner(new BufferedReader(new InputStreamReader(env.in())));
    }

    public Scanner getInputScanner() {
        return this.inputScanner;
    }

    public void pushScope() {
        varScopes.push(new HashMap<>());
        funcScopes.push(new HashMap<>());
    }

    public void popScope() {
        varScopes.pop();
        funcScopes.pop();
    }

    private HashMap<String, Object> findVarScope(String sym) {
        return varScopes.stream().filter(scope -> scope.containsKey(sym)).reduce((a, b) -> b).orElseThrow();
    }

    private HashMap<String, CallTarget> findFuncScope(String sym) {
        return funcScopes.stream().filter(scope -> scope.containsKey(sym)).reduce((a, b) -> b).orElseThrow();
    }

    public void setVar(String sym, Object value) {
        var scope = this.findVarScope(sym);
        scope.put(sym, value);
    }

    public Object getVar(String sym) {
        var scope = this.findVarScope(sym);
        return scope.get(sym);
    }

    public void defVar(String sym, Object value) {
        varScopes.peek().put(sym, value);
    }

    public CallTarget getFun(String sym) {
        var scope = this.findFuncScope(sym);
        return scope.get(sym);
    }

    public void defFun(String sym, CallTarget func) {
        funcScopes.peek().put(sym, func);
    }
}
