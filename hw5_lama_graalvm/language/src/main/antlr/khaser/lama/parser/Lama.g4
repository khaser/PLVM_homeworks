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
import khaser.lama.nodes.structs.*;
import khaser.lama.nodes.refs.*;

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

lama : scope_expr EOF;

scope_expr returns [LamaScopeNode result, List<LamaDefNode> defs, List<LamaFunDefNode> funDefs] :
    { $defs = new LinkedList(); $funDefs = new LinkedList(); }
    (
        var_defs { $defs.addAll($var_defs.result); }
        | fun_def { $funDefs.add(factory.createFunDef($fun_def.funName, $fun_def.body)); }
    )*
    expr_seq
    { $result = factory.createScope($defs, $funDefs, $expr_seq.result); }
    ;

// Definitions
fun_def returns [String funName, List<String> args, LamaScopeNode body] :
    'public'? 'fun'
    LIDENT { $funName = $LIDENT.getText(); }
    '('
    { $args = new LinkedList(); }
    (LIDENT { $args.add($LIDENT.getText()); }
        (',' LIDENT { $args.add($LIDENT.getText()); })*
    )?
    ')'
    { factory.setFunArgs($args); }
    '{'
    scope_expr { $body = $scope_expr.result; }
    '}'
    { factory.unsetFunArgs(); }
    ;

var_defs returns [List<LamaDefNode> result] :
    ('var'|'public') var_defs_seq { $result = $var_defs_seq.result; } ';';

var_defs_seq returns [List<LamaDefNode> result] :
    var_def_item { $result = new LinkedList(Arrays.asList($var_def_item.result)); }
    (',' var_def_item { $result.add($var_def_item.result); } )*;

var_def_item returns [LamaDefNode result] :
    LIDENT { $result = factory.createDef($LIDENT); }
    | (LIDENT '=' expr) { $result = factory.createDef($LIDENT, $expr.result); };

// Expressions
expr_seq returns [LamaExprNode result]:
    expr { $result = $expr.result; }
    (';' expr_seq { $result = new LamaSeqNode($expr.result, $expr_seq.result); })?
    ;

expr returns [LamaExprNode result] : expr_assign { $result = $expr_assign.result; };

expr_assign returns [LamaExprNode result] :
    dest=expr_ref ':=' val=expr_assign { $result = LamaAssignNodeGen.create($dest.result, $val.result); }
    | expr_disj { $result = $expr_disj.result; }
    ;

expr_disj returns [LamaExprNode result] :
    expr_conj { $result = $expr_conj.result; }
    (
        op='!!'
        expr_conj { $result = factory.createBinop($op, $result, $expr_conj.result); }
    )*
    ;

expr_conj returns [LamaExprNode result] :
    expr_comp { $result = $expr_comp.result; }
    (
        op='&&'
        expr_comp { $result = factory.createBinop($op, $result, $expr_comp.result); }
    )*
    ;

expr_comp returns [LamaExprNode result] :
    expr_add { $result = $expr_add.result; }
    (
        op=('=='|'!='|'<'|'<='|'>'|'>=')
        expr_add { $result = factory.createBinop($op, $result, $expr_add.result); }
    )*
    ;

expr_add returns [LamaExprNode result] :
    expr_mult { $result = $expr_mult.result; }
    (
        op=('+'|'-')
        expr_mult { $result = factory.createBinop($op, $result, $expr_mult.result); }
    )*
    ;

expr_mult returns [LamaExprNode result] :
    expr_dot_call { $result = $expr_dot_call.result; }
    (
        op=('*'|'/'|'%')
        expr_dot_call { $result = factory.createBinop($op, $result, $expr_dot_call.result); }
    )*
    ;

expr_dot_call returns [LamaExprNode result, String callTarget, List<LamaExprNode> args] :
    expr_member '.' LIDENT
    { $args = Arrays.asList($expr_member.result); $callTarget = $LIDENT.getText(); }
    (
        '('
        (expr { $args.add($expr.result); })?
        (',' expr { $args.add($expr.result); })*
        ')'
    )?
    { $result = factory.createCall($callTarget, $args); }
    | expr_member { $result = $expr_member.result; }
    ;

expr_member returns [LamaExprNode result] :
    expr_fun_call { $result = $expr_fun_call.result; }
    | expr_primary { $result = $expr_primary.result; }
    | arr=expr_member '[' idx=expr ']'
      { $result = LamaArrIdxNodeGen.create($arr.result, $idx.result); }
    | '-' expr_primary { $result = LamaNegNodeGen.create($expr_primary.result); }
    ;

expr_fun_call returns [LamaExprNode result, String callTarget, List<LamaExprNode> args] :
    { $args = new LinkedList(); }
    (LIDENT { $callTarget = $LIDENT.getText(); })
    '('
    (expr { $args.add($expr.result); })?
    (',' expr { $args.add($expr.result); })*
    ')'
    { $result = factory.createCall($callTarget, $args); }
    ;

expr_primary returns [LamaExprNode result] :
    DECIMAL { $result = factory.createDecimal($DECIMAL); }
    | STRING { $result = new LamaStringLiteralNode($STRING.getText().replaceAll("\"", "")); }
    | CHAR { $result = factory.createChar($CHAR); }
    | LIDENT { $result = factory.createRead($LIDENT); }
    | '(' scope_expr { $result = new LamaNestedScope($scope_expr.result); } ')'
    | 'skip' { $result = new LamaSkipNode(); }
    | if_expr { $result = $if_expr.result; }
    | while_expr { $result = $while_expr.result; }
    | for_expr { $result = $for_expr.result; }
    | array_expr { $result = $array_expr.result; }
    ;

expr_ref returns [LamaRefNode result] :
    LIDENT { $result = new LamaRefVarNode($LIDENT.getText()); }
    | expr_weak { $result = $expr_weak.result; }
    | 'if' pred=expr_seq 'then' thenRef=expr_ref 'else' elseRef=expr_ref 'fi'
      { $result = LamaRefIfNodeGen.create($pred.result, $thenRef.result, $elseRef.result); }
    | '(' left=expr ';' right=expr_ref ')'
      { $result = new LamaRefSeqNode($left.result, $right.result); }
    ;

expr_weak returns [LamaWeakNode result] :
    LIDENT { $result = new LamaWeakVarNode($LIDENT.getText()); }
    | dest=expr_weak '[' idx=expr ']'
      { $result = LamaWeakArrNodeGen.create($dest.result, $idx.result); }
    ;

array_expr returns [LamaExprNode result, List<LamaExprNode> els] :
    { $els = new LinkedList(); }
    '['
    (expr { $els.add($expr.result); })?
    (',' expr { $els.add($expr.result); })*
    ']'
    { $result = factory.createArrayObject($els); }
    ;

if_expr returns [LamaExprNode result] :
    'if' expr_seq 'then' scope_expr
    { $result = factory.createIf($expr_seq.result, $scope_expr.result); }
    (else_part { $result = factory.createIf($expr_seq.result, $scope_expr.result, $else_part.result); })?
    'fi'
    ;

else_part returns [LamaScopeNode result] :
    'elif' expr_seq 'then' scope_expr
    { $result = factory.wrapToScope(factory.createIf($expr_seq.result, $scope_expr.result)); }
    (else_part { $result = factory.wrapToScope(factory.createIf($expr_seq.result, $scope_expr.result, $else_part.result)); })?
    | 'else' scope_expr { $result = $scope_expr.result; }
    ;

while_expr returns [LamaExprNode result] :
    'while' expr_seq 'do' scope_expr 'od'
    { $result = new LamaWhileDoNode($expr_seq.result, $scope_expr.result); }
    | 'do' scope_expr 'while' expr_seq 'od'
    { $result = new LamaDoWhileNode($expr_seq.result, $scope_expr.result); }
    ;

for_expr returns [LamaExprNode result] :
    'for' init=scope_expr ',' cond=expr_seq ',' iter=expr_seq 'do'
    body=scope_expr
    { $result = new LamaForNode($init.result, $cond.result, $iter.result, $body.result); }
    'od'
    ;

fragment LETTER : [A-Z] | [a-z] | '_';
fragment DIGIT : [0-9];
DECIMAL : DIGIT+ ;

LIDENT : [a-z] (LETTER | DIGIT)*;
UIDENT : [A-Z] (LETTER | DIGIT)*;
STRING : '"' (~('"'))* '"';
CHAR : '\'' (~('\'')|'\\n'|'\\t') '\'';

WS : ' ' -> skip;
TAB : '\t' -> skip;
NEWLINE : '\n' -> skip;
