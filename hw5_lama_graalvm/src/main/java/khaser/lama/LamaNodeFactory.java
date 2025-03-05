package khaser.lama;

import java.util.ArrayList;
import java.util.List;
import java.lang.Integer;

import khaser.lama.nodes.funcs.LamaCallNode;
import khaser.lama.nodes.funcs.LamaFunctionDispatchNode;
import org.antlr.v4.runtime.Token;

import com.oracle.truffle.api.RootCallTarget;

import khaser.lama.nodes.*;
import khaser.lama.nodes.binops.*;
import khaser.lama.nodes.cfg.*;

public class LamaNodeFactory {

    private LamaScopeNode curScope;
    private LamaExprNode curScopeExpr = new LamaSkipNode();
    private ArrayList<LamaDefNode> curScopeDefs = new ArrayList();

    private ArrayList<LamaExprNode> curCallArgs = new ArrayList();
    private String curCallTarget;

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

    public LamaCallNode createCall() {
        LamaCallNode callNode = new LamaCallNode(new LamaFunctionDispatchNode(curCallTarget),
                                                 curCallArgs.toArray(new LamaExprNode[0]));
        curCallTarget = null;
        curCallArgs = new ArrayList();
        return callNode;
    }

    public void setCallTarget(Token builtinToken) {
        curCallTarget = builtinToken.getText();
    }

    public void addArgument(LamaExprNode expr) {
        curCallArgs.add(expr);
    }

    public RootCallTarget getCallTarget() {
        var rootNode = new LamaRootNode(curScope);
        return rootNode.getCallTarget();
    }

}
