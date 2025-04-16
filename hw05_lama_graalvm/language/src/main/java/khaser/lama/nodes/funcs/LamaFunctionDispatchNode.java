package khaser.lama.nodes.funcs;

import com.oracle.truffle.api.CallTarget;
import com.oracle.truffle.api.dsl.Cached;
import com.oracle.truffle.api.dsl.NodeField;
import com.oracle.truffle.api.dsl.Specialization;
import com.oracle.truffle.api.nodes.Node;
import khaser.lama.LamaContext;

@NodeField(name = "funName", type = String.class)
public abstract class LamaFunctionDispatchNode extends Node {
    public abstract Object executeDispatch(Object[] args);
    protected abstract String getFunName();

    @Specialization
    public Object direct(Object[] args,
                         @Cached(value = "getContext().getFun(getFunName())", neverDefault = true)
                         CallTarget cachedCallNode) {
        return cachedCallNode.call(args);
    }

    protected final LamaContext getContext() {
        return LamaContext.get(this);
    }
}
