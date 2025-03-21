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
import khaser.lama.nodes.cfg.pat_match.*;
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
        | fun_def { $funDefs.add($fun_def.result); }
    )*
    expr_seq
    { $result = factory.createScope($defs, $funDefs, $expr_seq.result); }
    ;

// Definitions
fun_def returns [LamaFunDefNode result] :
    'public'? 'fun'
    LIDENT { String funName = $LIDENT.getText(); }
    '('
    { List<String> args = new LinkedList(); }
    (LIDENT { args.add($LIDENT.getText()); }
        (',' LIDENT { args.add($LIDENT.getText()); })*
    )?
    ')'
    '{'
    scope_expr { $result = factory.createFunDef(funName, args, $scope_expr.result); }
    '}'
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
    | expr_cons { $result = $expr_cons.result; }
    ;

expr_cons returns [LamaExprNode result] :
    head=expr_disj ':' tail=expr_cons { $result = LamaListConsNodeGen.create($head.result, $tail.result); }
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
    DECIMAL { $result = new LamaConstIntNode(factory.dec2Int($DECIMAL)); }
    | STRING { $result = new LamaStringCreateNode($STRING.getText().replaceAll("\"", "")); }
    | CHAR { $result = new LamaConstIntNode(factory.char2Int($CHAR)); }
    | LIDENT { $result = new LamaReadNode($LIDENT.getText()); }
    | 'skip' { $result = new LamaSkipNode(); }
    | '(' scope_expr { $result = new LamaNestedScope($scope_expr.result); } ')'
    | if_expr { $result = $if_expr.result; }
    | while_expr { $result = $while_expr.result; }
    | for_expr { $result = $for_expr.result; }
    | array_expr { $result = $array_expr.result; }
    | case_expr { $result = $case_expr.result; }
    | s_expr { $result = $s_expr.result; }
    | list_expr { $result = $list_expr.result; }
    | 'true' { $result = new LamaConstIntNode(1); }
    | 'false' { $result = new LamaConstIntNode(0); }
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

// Structures
array_expr returns [LamaExprNode result, List<LamaExprNode> els] :
    { $els = new LinkedList(); }
    '['
    (expr { $els.add($expr.result); })?
    (',' expr { $els.add($expr.result); })*
    ']'
    { $result = factory.createArrayObject($els); }
    ;

s_expr returns [LamaExprNode result] :
    { var els = new LinkedList<LamaExprNode>(); }
    UIDENT
    (
    '('
    expr { els.add($expr.result); }
    (',' expr { els.add($expr.result); })*
    ')'
    )?
    { $result = factory.createSexprObject($UIDENT.getText(), els); }
    ;

list_expr returns [LamaExprNode result] :
    { List<LamaExprNode> els = new LinkedList<>(); }
    '{'
    (expr { els.add($expr.result); })?
    (',' expr { els.add($expr.result); })*
    '}'
    { $result = factory.createListObject(els); }
    ;

// Control flow
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

// Pattern matching
case_expr returns [LamaExprNode result] :
    'case' scrut=expr 'of'
    { var branches = new LinkedList<LamaCaseBranch>(); }
    case_branch { branches.add($case_branch.result); }
    (
        '|' case_branch { branches.add($case_branch.result); }
    )*
    'esac'
    { $result = new LamaCaseNode($scrut.result, branches.toArray(new LamaCaseBranch[0])); }
    ;

case_branch returns [LamaCaseBranch result] :
    pat=case_pattern '->' scope=scope_expr
    { $result = new LamaCaseBranch($pat.result, $scope.result); }
    ;

case_pattern returns [LamaPattern result] :
    pat=case_simpl_pattern { $result = $pat.result; }
    | case_cons_pattern { $result = $case_cons_pattern.result; }
    ;

case_cons_pattern returns [LamaPattern result] :
    head=case_simpl_pattern ':' tail=case_pattern
    { $result = new LamaConsPattern($head.result, $tail.result); }
    ;

case_simpl_pattern returns [LamaPattern result] :
    DECIMAL { $result = new LamaIntPattern(factory.dec2Int($DECIMAL)); }
    | '-' DECIMAL { $result = new LamaIntPattern(-factory.dec2Int($DECIMAL)); }
    | CHAR { $result = new LamaIntPattern(factory.char2Int($CHAR)); }
    | '_' { $result = new LamaWildcardPattern(); }
    | LIDENT { $result = new LamaBindPattern($LIDENT.getText(), new LamaWildcardPattern()); }
    | LIDENT '@' case_pattern { $result = new LamaBindPattern($LIDENT.getText(), $case_pattern.result); }
    | case_array_pattern { $result = $case_array_pattern.result; }
    | case_sexpr_pattern { $result = $case_sexpr_pattern.result; }
    | case_list_pattern { $result = $case_list_pattern.result; }
    | '(' case_pattern ')' { $result = $case_pattern.result; }
    | 'true' { $result = new LamaIntPattern(1); }
    | 'false' { $result = new LamaIntPattern(0); }
    ;
    // TODO: string pattern

case_array_pattern returns [LamaPattern result] :
    { var args = new LinkedList<LamaPattern>(); }
    '['
    (
        case_pattern { args.add($case_pattern.result); }
        (',' case_pattern { args.add($case_pattern.result); })*
    )?
    ']'
    { $result = new LamaArrayPattern(args.toArray(new LamaPattern[0])); }
    ;

case_sexpr_pattern returns [LamaPattern result] :
    { var args = new LinkedList<LamaPattern>(); }
    UIDENT
    (
    '('
        case_pattern { args.add($case_pattern.result); }
        (',' case_pattern { args.add($case_pattern.result); })*
    ')'
    )?
    { $result = new LamaSexprPattern($UIDENT.getText(), args.toArray(new LamaPattern[0])); }
    ;

case_list_pattern returns [LamaPattern result] :
    { var args = new LinkedList<LamaPattern>(); }
    '{'
    (
        case_pattern { args.add($case_pattern.result); }
        (',' case_pattern { args.add($case_pattern.result); })*
    )?
    '}'
    { $result = new LamaListPattern(args.toArray(new LamaPattern[0])); }
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
BLOCK_COMMENT : '(*' .*? '*)' -> skip;
LINE_COMMENT : '--' ~[\r\n]* -> skip;

