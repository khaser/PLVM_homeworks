package khaser.lama.nodes.builtins;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.NodeInfo;

import java.util.Scanner;

import khaser.lama.nodes.LamaNode;

@NodeInfo(shortName = "read")
public class LamaBuiltinReadNode extends LamaNode {

    public LamaBuiltinReadNode() {
    }

    @Override
    public Integer execute(VirtualFrame frame) {
        var scan = new Scanner(getContext().getInput());
        return Integer.parseInt(scan.nextLine());
    }

}