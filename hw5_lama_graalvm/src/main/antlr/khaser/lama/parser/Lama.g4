grammar Lama;

@lexer::header {
package khaser.lama.parser;
}

@parser::header {
package khaser.lama.parser;

import khaser.lama.parser.LamaLexer;
import khaser.lama.LamaNodeFactory;

import khaser.lama.nodes.*;
import khaser.lama.nodes.binops.*;
import khaser.lama.nodes.cfg.*;

import java.io.Reader;
import java.util.Arrays;
import java.util.LinkedList;
import java.io.IOException;

import com.oracle.truffle.api.RootCallTarget;
}

@parser::members {
private LamaNodeFactory factory;

public static RootCallTarget parseLama(Reader source) throws IOException {
    LamaLexer lexer = new LamaLexer(CharStreams.fromReader(source));
    LamaParser parser = new LamaParser(new CommonTokenStream(lexer));
    parser.factory = new LamaNodeFactory();
    parser.lama();
    return parser.factory.getCallTarget();
}
}

lama : scope_expr ;

scope_expr returns [LamaScopeNode result] :
    (var_defs { factory.addScopeDefs($var_defs.result); })*
    (expr { factory.setScopeExpr($expr.result); })
    { $result = factory.createScope(); };

// Definitions
var_defs returns [List<LamaDefNode> result] :
    ('var'|'public') var_defs_seq { $result = $var_defs_seq.result; } ';';

var_defs_seq returns [List<LamaDefNode> result] :
    var_def_item { $result = new LinkedList(Arrays.asList($var_def_item.result)); }
    (',' var_def_item { $result.add($var_def_item.result); } )*;

var_def_item returns [LamaDefNode result] :
    LIDENT { $result = factory.createDef($LIDENT); }
    | (LIDENT '=' expr) { $result = factory.createDef($LIDENT, $expr.result); };

// Expressions
expr_seq : expr ';' expr_seq;
expr returns [LamaExprNode result] : expr_add { $result = $expr_add.result; };

expr_add returns [LamaExprNode result] :
    expr_mult { $result = $expr_mult.result; }
    (
        op=('+'|'-')
        expr_mult { $result = factory.createBinop($op, $result, $expr_mult.result); }
    )*
    ;

expr_mult returns [LamaExprNode result] :
    expr_member { $result = $expr_member.result; }
    (
        op=('*'|'/'|'%')
        expr_member { $result = factory.createBinop($op, $result, $expr_member.result); }
    )*
    ;

expr_member returns [LamaExprNode result] :
    DECIMAL { $result = factory.createDecimal($DECIMAL); }
    | LIDENT { $result = factory.createRead($LIDENT); }
    ;

fragment LETTER : [A-Z] | [a-z] | '_';
fragment DIGIT : [0-9];
DECIMAL : '-'? DIGIT+ ;

LIDENT : [a-z] (LETTER | DIGIT)*;
UIDENT : [A-Z] (LETTER | DIGIT)*;
WS : ' ' -> skip;
NEWLINE : '\n' -> skip;
