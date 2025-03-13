package khaser.lama.nodes.funcs;

import com.oracle.truffle.api.frame.VirtualFrame;

import khaser.lama.nodes.LamaExprNode;

public class LamaReadArgNode extends LamaExprNode {

    private Integer idx;

    public LamaReadArgNode(Integer idx) {
        this.idx = idx;
    }

    @Override
    public Integer execute(VirtualFrame frame) {
        Object[] arguments = frame.getArguments();
        return (Integer) arguments[this.idx];
    }
}
