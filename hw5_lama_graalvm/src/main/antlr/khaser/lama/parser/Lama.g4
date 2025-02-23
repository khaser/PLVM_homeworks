grammar Lama;

@lexer::header {
package khaser.lama.parser;
}

@parser::header {
package khaser.lama.parser;

import khaser.lama.parser.LamaLexer;

import khaser.lama.nodes.LamaExprNode;
import khaser.lama.LamaNodeFactory;
import java.io.Reader;
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

// stmt : 'skip'
//      | expr_add
//      | stmt ';' stmt
//      | 'read' '(' LIDENT ')'
//      | 'write' '(' expr ')'
//      ;

lama : expr_add '\n' ;

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

expr_member returns [LamaExprNode result]
    : DECIMAL { $result = factory.createDecimal($DECIMAL); }
    //| LIDENT { $result = factory.createRead($LIDENT); }
    ;

fragment LETTER : [A-Z] | [a-z] | '_';
fragment DIGIT : [0-9];
DECIMAL : '-'? DIGIT+ ;

LIDENT : [a-z] (LETTER | DIGIT)*;
UIDENT : [A-Z] (LETTER | DIGIT)*;
