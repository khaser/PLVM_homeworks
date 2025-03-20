package khaser.lama;

import com.oracle.truffle.api.CallTarget;

import java.util.HashMap;
import java.util.Optional;
import java.util.Stack;

public class LamaFrame {
    public final Stack<HashMap<String, Object[]>> vars = new Stack<>();
    public final Stack<HashMap<String, CallTarget>> funcs = new Stack<>();

    public LamaFrame() {
    }

    public void pushScope() {
        vars.push(new HashMap<>());
        funcs.push(new HashMap<>());
    }

    public void popScope() {
        vars.pop();
        funcs.pop();
    }

    Optional<HashMap<String, Object[]>> findVarScope(String sym) {
        return vars.stream().filter(scope -> scope.containsKey(sym)).reduce((a, b) -> b);
    }

    Optional<HashMap<String, CallTarget>> findFuncScope(String sym) {
        return funcs.stream().filter(scope -> scope.containsKey(sym)).reduce((a, b) -> b);
    }

    public void defFun(String sym, CallTarget func) {
        funcs.peek().put(sym, func);
    }

    public void defVar(String sym, Object val) {
        vars.peek().put(sym, LamaContext.wrapRef(val));
    }
}
