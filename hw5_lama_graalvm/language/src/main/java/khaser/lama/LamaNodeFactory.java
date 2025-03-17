package khaser.lama;

import java.util.ArrayList;
import java.util.List;
import java.lang.Integer;

import khaser.lama.nodes.funcs.LamaCallNode;
import khaser.lama.nodes.funcs.LamaFunctionDispatchNode;
import khaser.lama.nodes.funcs.LamaReadArgNode;
import khaser.lama.nodes.structs.LamaArrCreateNode;
import org.antlr.v4.runtime.Token;

import com.oracle.truffle.api.RootCallTarget;

import khaser.lama.nodes.*;
import khaser.lama.nodes.binops.*;
import khaser.lama.nodes.cfg.*;

public class LamaNodeFactory {

    private LamaScopeNode curScope;

    private List<String> curFunArgNames;

    public LamaNodeFactory() {
    }


    public LamaExprNode createBinop(Token opToken, LamaExprNode leftNode, LamaExprNode rightNode) {
        if (leftNode == null || rightNode == null) {
            return null;
        }

        switch (opToken.getText()) {
            case "+":
                return LamaAddNodeGen.create(leftNode, rightNode);
            case "*":
                return LamaMulNodeGen.create(leftNode, rightNode);
             case "-":
                 return LamaSubNodeGen.create(leftNode, rightNode);
            case "/":
                return LamaDivNodeGen.create(leftNode, rightNode);
            case "%":
                return LamaModNodeGen.create(leftNode, rightNode);
            case "==":
                return LamaEqNodeGen.create(leftNode, rightNode);
            case "!=":
                return LamaLnotNodeGen.create(LamaEqNodeGen.create(leftNode, rightNode));
            case "<":
                return LamaLeNodeGen.create(leftNode, rightNode);
            case "<=":
                return LamaLeqNodeGen.create(leftNode, rightNode);
            case ">":
                return LamaLeNodeGen.create(rightNode, leftNode);
            case ">=":
                return LamaLeqNodeGen.create(rightNode, leftNode);
            case "&&":
                return LamaLandNodeGen.create(leftNode, rightNode);
            case "!!":
                return LamaLorNodeGen.create(leftNode, rightNode);
            default:
                throw new RuntimeException("unexpected operation: " + opToken.getText());
        }
    }

    public LamaExprNode createDecimal(Token literalToken) {
        return new LamaConstIntNode(Integer.parseInt(literalToken.getText()));
    }

    public void setFunArgs(List<String> args) {
        curFunArgNames = args;
    }

    public void unsetFunArgs() {
        curFunArgNames = new ArrayList<>();
    }

    public LamaScopeNode createScope(List<LamaDefNode> scopeDefs,
                                     List<LamaFunDefNode> scopeFunDefs,
                                     LamaExprNode expr) {
        curScope = new LamaScopeNode(scopeDefs.toArray(new LamaDefNode[0]),
                                     scopeFunDefs.toArray(new LamaFunDefNode[0]),
                                     expr);
        return curScope;
    }

    public LamaDefNode createDef(Token sym, LamaExprNode expr) {
        return new LamaDefNode(sym.getText(), expr);
    }

    public LamaDefNode createDef(Token sym) {
        return createDef(sym, new LamaConstIntNode(0));
    }

    public LamaExprNode createSeq(LamaExprNode left, LamaExprNode right) {
        return new LamaSeqNode(left, right);
    }

    public LamaFunDefNode createFunDef(String funName, LamaScopeNode funBody) {
        return new LamaFunDefNode(funName, funBody);
    }

    public LamaExprNode createRead(Token varNameToken) {
        String varName = varNameToken.getText();
        if (curFunArgNames != null && curFunArgNames.contains(varName)) {
            return new LamaReadArgNode(curFunArgNames.indexOf(varName));
        } else {
            return new LamaReadNode(varName);
        }
    }

    public LamaCallNode createCall(String callTarget, List<LamaExprNode> args) {
        return new LamaCallNode(new LamaFunctionDispatchNode(callTarget),
                                args.toArray(new LamaExprNode[0]));
    }

    public RootCallTarget getCallTarget() {
        var rootNode = new LamaRootNode(curScope);
        return rootNode.getCallTarget();
    }

    public LamaScopeNode wrapToScope(LamaExprNode node) {
        return new LamaScopeNode(new LamaDefNode[0], new LamaFunDefNode[0], node);
    }

    public LamaExprNode createArrayObject(List<LamaExprNode> els) {
        return new LamaArrCreateNode(els.toArray(new LamaExprNode[0]));
    }

    public LamaExprNode createIf(LamaExprNode pred, LamaScopeNode thenScope, LamaScopeNode elseScope) {
        return new LamaIfNode(pred, thenScope, elseScope);
    }

    public LamaExprNode createIf(LamaExprNode pred, LamaScopeNode thenScope) {
        var skipScope = new LamaScopeNode(new LamaDefNode[0], new LamaFunDefNode[0], new LamaSkipNode());
        return new LamaIfNode(pred, thenScope, skipScope);
    }

//    public createArr() {
//        LamaArrReadGe
//    }
}
