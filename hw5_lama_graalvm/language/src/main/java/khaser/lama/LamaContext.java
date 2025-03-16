package khaser.lama;

import com.oracle.truffle.api.CallTarget;
import com.oracle.truffle.api.TruffleLanguage;
import com.oracle.truffle.api.nodes.Node;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.LinkedList;
import java.util.Scanner;
import java.util.HashMap;

public final class LamaContext {

    private static final TruffleLanguage.ContextReference<LamaContext> REF =
          TruffleLanguage.ContextReference.create(LamaLanguage.class);
    private final Scanner inputScaner;
    public static LamaContext get(Node node) {
        return REF.get(node);
    }

    private final LinkedList<HashMap<String, Integer>> varScopes = new LinkedList<>();
    private final LinkedList<HashMap<String, CallTarget>> funcScopes = new LinkedList<>();


    public LamaContext(TruffleLanguage.Env env) {
        this.inputScaner = new Scanner(new BufferedReader(new InputStreamReader(env.in())));
    }

    public Scanner getInputScaner() {
        return this.inputScaner;
    }

    public void pushScope() {
        varScopes.push(new HashMap<>());
        funcScopes.push(new HashMap<>());
    }

    public void popScope() {
        varScopes.pop();
        funcScopes.pop();
    }

    private HashMap<String, Integer> findVarScope(String sym) {
        return varScopes.stream().filter(scope -> scope.containsKey(sym)).findFirst().orElseThrow();
    }

    private HashMap<String, CallTarget> findFuncScope(String sym) {
        return funcScopes.stream().filter(scope -> scope.containsKey(sym)).findFirst().orElseThrow();
    }

    public void setVar(String sym, int value) {
        varScopes.getLast().put(sym, value);
    }

    public int getVar(String sym) {
        var scope = this.findVarScope(sym);
        return scope.get(sym);
    }

    public CallTarget getFun(String sym) {
        var scope = this.findFuncScope(sym);
        return scope.get(sym);
    }

    public void setFun(String sym, CallTarget func) {
        funcScopes.getLast().put(sym, func);
    }
}
