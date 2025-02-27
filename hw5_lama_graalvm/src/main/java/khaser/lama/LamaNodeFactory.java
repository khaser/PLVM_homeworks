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
import khaser.lama.nodes.cfg.*;
import khaser.lama.LamaLanguage;

public class LamaNodeFactory {

    private LamaScopeNode curScope;
    private LamaExprNode curScopeExpr = new LamaSkipNode();
    private ArrayList<LamaDefNode> curScopeDefs = new ArrayList();

    public LamaNodeFactory() {
    }

    public LamaExprNode createBinop(Token opToken, LamaExprNode leftNode, LamaExprNode rightNode) {
        if (leftNode == null || rightNode == null) {
            return null;
        }

        switch (opToken.getText()) {
            case "+":
                return new LamaAddNode(leftNode, rightNode);
            // case "-":
            //     result = LamaSubNodeGen.create(leftUnboxed, rightUnboxed);
            //     break;
            // case "*":
            //     result = LamaMulNodeGen.create(leftUnboxed, rightUnboxed);
            //     break;
            default:
                throw new RuntimeException("unexpected operation: " + opToken.getText());
        }
    }

    public LamaExprNode createDecimal(Token literalToken) {
        return new LamaIntNode(Integer.parseInt(literalToken.getText()));
    }

    public void addScopeDefs(List<LamaDefNode> defs) {
        System.out.println("Scope defs added\n");
        curScopeDefs.addAll(defs);
    }

    public void setScopeExpr(LamaExprNode expr) {
        System.out.println("Scope expr added\n");
        curScopeExpr = expr;
    }

    public LamaScopeNode createScope() {
        curScope = new LamaScopeNode(curScopeDefs.toArray(new LamaDefNode[0]), curScopeExpr);
        curScopeDefs = new ArrayList();
        curScopeExpr = new LamaSkipNode();
        System.out.println("New scope created\n");
        return curScope;
    }

    public LamaDefNode createDef(Token sym, LamaExprNode expr) {
        return new LamaDefNode(sym.getText(), expr);
    }

    public LamaDefNode createDef(Token sym) {
        return createDef(sym, new LamaIntNode(0));
    }

    public LamaReadNode createRead(Token sym) {
        return new LamaReadNode(sym.getText());
    }

    public RootCallTarget getCallTarget() {
        var rootNode = new LamaRootNode(curScope);
        return rootNode.getCallTarget();
    }

}
