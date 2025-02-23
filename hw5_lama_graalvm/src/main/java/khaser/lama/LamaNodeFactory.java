package khaser.lama;

import java.math.BigInteger;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.lang.Integer;

import org.antlr.v4.runtime.Parser;
import org.antlr.v4.runtime.Token;

import com.oracle.truffle.api.RootCallTarget;
import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.frame.FrameSlotKind;
import com.oracle.truffle.api.source.Source;
import com.oracle.truffle.api.source.SourceSection;
import com.oracle.truffle.api.strings.TruffleString;
import com.oracle.truffle.api.RootCallTarget;

import khaser.lama.nodes.*;
import khaser.lama.nodes.binops.*;
import khaser.lama.LamaLanguage;

public class LamaNodeFactory {

    private LamaExprNode curExpr;

    public LamaNodeFactory() {
    }

    public LamaExprNode createBinop(Token opToken, LamaExprNode leftNode, LamaExprNode rightNode) {
        if (leftNode == null || rightNode == null) {
            return null;
        }

        final LamaExprNode result;
        switch (opToken.getText()) {
            case "+":
                result = new LamaAddNode(leftNode, rightNode);
                break;
            // case "-":
            //     result = LamaSubNodeGen.create(leftUnboxed, rightUnboxed);
            //     break;
            // case "*":
            //     result = LamaMulNodeGen.create(leftUnboxed, rightUnboxed);
            //     break;
            default:
                throw new RuntimeException("unexpected operation: " + opToken.getText());
        }
        curExpr = result;
        return result;
    }

    public LamaExprNode createDecimal(Token literalToken) {
        return new LamaIntNode(Integer.parseInt(literalToken.getText()));
    }

    public RootCallTarget getCallTarget() {
        var rootNode = new LamaRootNode(curExpr);
        return rootNode.getCallTarget();
    }

    // public LamaExprNode createReadProperty(SLExpressionNode receiverNode, SLExpressionNode nameNode) {
    //     if (receiverNode == null || nameNode == null) {
    //         return null;
    //     }
    //
    //     final SLExpressionNode result = SLReadPropertyNodeGen.create(receiverNode, nameNode);
    //
    //     final int startPos = receiverNode.getSourceCharIndex();
    //     final int endPos = nameNode.getSourceEndIndex();
    //     result.setSourceSection(startPos, endPos - startPos);
    //     result.addExpressionTag();
    //
    //     return result;
    // }

}
