package khaser.lama;

import java.util.*;
import java.lang.Integer;

import khaser.lama.nodes.funcs.LamaCallNode;
import khaser.lama.nodes.funcs.LamaFunctionDispatchNode;
import khaser.lama.nodes.funcs.LamaFunctionDispatchNodeGen;
import khaser.lama.nodes.funcs.LamaReadArgNode;
import khaser.lama.nodes.structs.LamaArrCreateNode;
import khaser.lama.nodes.structs.LamaListCreateNode;
import khaser.lama.nodes.structs.LamaSexprCreateNode;
import org.antlr.v4.runtime.Token;

import com.oracle.truffle.api.RootCallTarget;

import khaser.lama.nodes.*;
import khaser.lama.nodes.binops.*;
import khaser.lama.nodes.cfg.*;

public class LamaNodeFactory {

    private LamaScopeNode curScope;


    public LamaNodeFactory() {
    }

    public LamaExprNode createBinop(Token opToken, LamaExprNode leftNode, LamaExprNode rightNode) {
        if (leftNode == null || rightNode == null) {
            return null;
        }

        return switch (opToken.getText()) {
            case "+" -> LamaAddNodeGen.create(leftNode, rightNode);
            case "*" -> LamaMulNodeGen.create(leftNode, rightNode);
            case "-" -> LamaSubNodeGen.create(leftNode, rightNode);
            case "/" -> LamaDivNodeGen.create(leftNode, rightNode);
            case "%" -> LamaModNodeGen.create(leftNode, rightNode);
            case "==" -> LamaEqNodeGen.create(leftNode, rightNode);
            case "!=" -> LamaLnotNodeGen.create(LamaEqNodeGen.create(leftNode, rightNode));
            case "<" -> LamaLeNodeGen.create(leftNode, rightNode);
            case "<=" -> LamaLeqNodeGen.create(leftNode, rightNode);
            case ">" -> LamaLeNodeGen.create(rightNode, leftNode);
            case ">=" -> LamaLeqNodeGen.create(rightNode, leftNode);
            case "&&" -> LamaLandNodeGen.create(leftNode, rightNode);
            case "!!" -> LamaLorNodeGen.create(leftNode, rightNode);
            default -> throw new RuntimeException("unexpected operation: " + opToken.getText());
        };
    }

    public int dec2Int(Token literalToken) {
        return Integer.parseInt(literalToken.getText());
    }

    public int char2Int(Token literalToken) {
        String token = literalToken.getText().replaceAll("\'", "");
        if (Objects.equals(token, "\\n")) {
            return '\n';
        } else if (Objects.equals(token, "\\t")) {
            return '\t';
        } else {
            return token.charAt(0);
        }
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

    public LamaFunDefNode createFunDef(String funName, List<String> largs, LamaScopeNode funBody) {
        String[] args = largs.toArray(new String[0]);
        List<LamaDefNode> defs = new LinkedList<>();
        for (int i = 0; i < args.length; ++i) {
            defs.add(new LamaDefNode(args[i], new LamaReadArgNode(i)));
        }
        var wrappedBody = new LamaScopeNode(defs.toArray(new LamaDefNode[0]),
                                            new LamaFunDefNode[0],
                                            new LamaNestedScope(funBody));
        return new LamaFunDefNode(funName, wrappedBody);
    }

    public LamaCallNode createCall(String callTarget, List<LamaExprNode> args) {
        return new LamaCallNode(LamaFunctionDispatchNodeGen.create(callTarget),
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

    public LamaExprNode createSexprObject(String ident, List<LamaExprNode> args) {
        return new LamaSexprCreateNode(ident, args.toArray(new LamaExprNode[0]));
    }

    public LamaExprNode createListObject(List<LamaExprNode> els) {
        return new LamaListCreateNode(els.toArray(new LamaExprNode[0]));
    }

    public LamaExprNode createIf(LamaExprNode pred, LamaScopeNode thenScope, LamaScopeNode elseScope) {
        return new LamaIfNode(pred, thenScope, elseScope);
    }

    public LamaExprNode createIf(LamaExprNode pred, LamaScopeNode thenScope) {
        var skipScope = new LamaScopeNode(new LamaDefNode[0], new LamaFunDefNode[0], new LamaSkipNode());
        return new LamaIfNode(pred, thenScope, skipScope);
    }

}
