#include <iostream>
#include "CST.h"
#include "List.h"
#include "Token.h"

#define DEBUG 0

ListNode<Token>* error_token = nullptr;

CSTNode::CSTNode(int _type, std::string _content) {
  type = _type;
  content = _content;
}

CSTNode* prog(ListNode<Token>* &token) {
  INIT(PROG)
  ENTER_INFO(PROG)
  if (token == nullptr) {
    NONTERMINAL_NS(_epsilon, fail, success)
  }
rule1:
  // <prog> ::= <decl>
  NONTERMINAL_NS(decl, rule2, rule3)
rule2:
  // <prog> ::= <def>
  RESTORE()
  NONTERMINAL_NS(def, fail, rule4)
rule3:
rule4:
  // <prog>::= <decl> <prog>
  //         | <def> <prog>
  NONTERMINAL_NS(prog, success, success)
success:
  SUCCESS()
fail:
  FAIL()
}

CSTNode* decl(ListNode<Token>* &token) {
  INIT(DECL)
  ENTER_INFO(DECL)
rule1:
  // <decl> ::= <type> <id> ( ) ;
  NONTERMINAL_N(type, rule5)
  NONTERMINAL_N(id, fail)
  NONTERMINAL_N(_left_paren, rule3)
  NONTERMINAL_N(_right_paren, rule2)
  NONTERMINAL_NS(_semicolon, fail, success)
rule2:
  // <decl> ::= <type> <id> ( <arg_list> ) ;
  NONTERMINAL_N(arg_list, fail)
  NONTERMINAL_N(_right_paren, fail)
  NONTERMINAL_NS(_semicolon, fail, success)
rule3:
  // <decl> ::= <type> <id> ;
  NONTERMINAL_NS(_semicolon, rule4, success)
rule4:
  // <decl> ::= <type> <id> = <expr> ;
  NONTERMINAL_N(_equals, fail)
  NONTERMINAL_N(expr, fail)
  NONTERMINAL_NS(_semicolon, fail, success)
rule5:
  // <decl> ::= struct <id> ;
  RESTORE()
  NONTERMINAL_N(_struct, rule6)
  NONTERMINAL_N(id, fail)
  NONTERMINAL_NS(_semicolon, fail, success)
rule6:
  // <decl> ::= enum { <enum_list> } ;
  RESTORE()
  NONTERMINAL_N(_enum, fail)
  NONTERMINAL_N(_left_brace, fail)
  NONTERMINAL_N(enum_list, fail)
  NONTERMINAL_N(_right_brace, fail)
  NONTERMINAL_NS(_semicolon, fail, success)
success:
  SUCCESS()
fail:
  FAIL()
}

CSTNode* arg_list(ListNode<Token>* &token) {
  INIT(ARG_LIST)
  ENTER_INFO(ARG_LIST)
rule1:
  // <arg_list> ::= <type> <id>
  NONTERMINAL_N(type, fail)
  NONTERMINAL_NS(id, fail, rule2)
rule2:
  // <arg_list> ::= <type> <id> , <arg_list>
  NONTERMINAL_N(_comma, success)
  NONTERMINAL_NS(arg_list, fail, success)
success:
  SUCCESS()
fail:
  FAIL()
}

CSTNode* enum_list(ListNode<Token>* &token) {
  INIT(ENUM_LIST)
  ENTER_INFO(ENUM_LIST)
rule1:
  // <enum_list> ::= <id> = <number>
  NONTERMINAL_N(id, fail)
  NONTERMINAL_N(_equals, rule2)
  NONTERMINAL_NS(number, fail, rule3)
rule2:
  // <enum_list> ::= <id>
  goto rule4;
rule3:
rule4:
  // <enum_list> ::= <id> = <number> , <enum_list>
  //               | <id> , <enum_list>
  NONTERMINAL_N(_comma, success)
  NONTERMINAL_NS(enum_list, fail, success)
success:
  SUCCESS()
fail:
  FAIL()
}

CSTNode* def(ListNode<Token>* &token) {
  INIT(DEF)
  ENTER_INFO(DEF)
rule1:
  // <def> ::= struct <id> { <mem_decl_list> } ;
  NONTERMINAL_N(_struct, rule2)
  NONTERMINAL_N(id, fail)
  NONTERMINAL_N(_left_brace, fail)
  NONTERMINAL_N(mem_decl_list, fail)
  NONTERMINAL_N(_right_brace, fail)
  NONTERMINAL_NS(_semicolon, fail, success)
rule2:
  // <def> ::= <type> <id> ( ) { <stmt_list> }
  NONTERMINAL_N(type, fail)
  NONTERMINAL_N(id, fail)
  NONTERMINAL_N(_left_paren, fail)
  NONTERMINAL_N(_right_paren, rule3)
  NONTERMINAL_N(_left_brace, fail)
  NONTERMINAL_N(stmt_list, fail)
  NONTERMINAL_NS(_right_brace, fail, success)
rule3:
  // <def> ::= <type> <id> ( <arg_list> ) { <stmt_list> }
  NONTERMINAL_N(arg_list, fail)
  NONTERMINAL_N(_right_paren, fail)
  NONTERMINAL_N(_left_brace, fail)
  NONTERMINAL_N(stmt_list, fail)
  NONTERMINAL_NS(_right_brace, fail, success)
success:
  SUCCESS()
fail:
  FAIL()
}

CSTNode* mem_decl_list(ListNode<Token>* &token) {
  INIT(MEM_DECL_LIST)
  ENTER_INFO(MEM_DECL_LIST)
rule1:
  // <mem_decl_list> ::= <type> <id> ; <mem_decl_list>
  NONTERMINAL_N(type, rule2)
  NONTERMINAL_N(id, rule2)
  NONTERMINAL_N(_semicolon, rule2)
  NONTERMINAL_NS(mem_decl_list, fail, success)
rule2:
  // <mem_decl_list> ::= epsilon
  NONTERMINAL_NS(_epsilon, fail, success)
success:
  SUCCESS()
fail: // should never reach here
  FAIL()
}

CSTNode* stmt_list(ListNode<Token>* &token) {
  INIT(STMT_LIST)
  ENTER_INFO(STMT_LIST)
rule1:
  // <stmt_list> ::= <stmt> <stmt_list>
  NONTERMINAL_N(stmt, rule2)
  NONTERMINAL_NS(stmt_list, fail, success)
rule2:
  // <stmt_list> ::= epsilon
  NONTERMINAL_NS(_epsilon, fail, success)
success:
  SUCCESS()
fail: // should never reach here
  FAIL()
}

CSTNode* stmt(ListNode<Token>* &token) {
  INIT(STMT)
  ENTER_INFO(STMT)
rule1:
  // <stmt> ::= ;
  NONTERMINAL_NS(_semicolon, rule2, success)
rule2:
  // <stmt> ::= <simple>
  RESTORE()
  NONTERMINAL_NS(simple, rule3, success)
rule3:
  // <stmt> ::= <if_stmt>
  RESTORE()
  NONTERMINAL_NS(if_stmt, rule4, success)
rule4:
  // <stmt> ::= <for_stmt>
  RESTORE()
  NONTERMINAL_NS(for_stmt, rule5, success)
rule5:
  // <stmt> ::= <jump_stmt>
  RESTORE()
  NONTERMINAL_NS(jump_stmt, rule6, success)
rule6:
  // <stmt> ::= <return_stmt>
  RESTORE()
  NONTERMINAL_NS(return_stmt, rule7, success)
rule7:
  // <stmt> ::= { <stmt_list> }
  RESTORE()
  NONTERMINAL_N(_left_brace, fail)
  NONTERMINAL_N(stmt_list, fail)
  NONTERMINAL_NS(_right_brace, fail, success)
success:
  SUCCESS()
fail:
  FAIL()
}

CSTNode* simple(ListNode<Token>* &token) {
  INIT(SIMPLE)
  ENTER_INFO(SIMPLE)
rule1:
  // <simple> ::= <expr> ;
  NONTERMINAL_N(expr, rule2)
  NONTERMINAL_NS(_semicolon, rule2, success)
rule2:
  // <simple> ::= <type> <id> = <expr> ;
  RESTORE()
  NONTERMINAL_N(type, fail)
  NONTERMINAL_N(id, fail)
  NONTERMINAL_N(_equals, rule3)
  NONTERMINAL_N(expr, fail)
  NONTERMINAL_NS(_semicolon, fail, success)
rule3:
  // <simple> ::= <type> <id> ;
  RESTORE()
  NONTERMINAL_N(type, fail)
  NONTERMINAL_N(id, fail)
  NONTERMINAL_NS(_semicolon, fail, success)
success:
  SUCCESS()
fail:
  FAIL()
}

CSTNode* assign_op(ListNode<Token>* &token) {
  ENTER_INFO(ASSIGN_OP)
rule1:
  // <assign_op> ::= =
  TERMINAL_CONTENT_NF("=", ASSIGN_OP,  rule2, fail)
rule2:
  // <assign_op> ::= +=
  TERMINAL_CONTENT_NF("+=", ASSIGN_OP, rule3, fail)
rule3:
  // <assign_op> ::= -=
  TERMINAL_CONTENT_NF("-=", ASSIGN_OP, rule4, fail)
rule4:
  // <assign_op> ::= *=
  TERMINAL_CONTENT_NF("*=", ASSIGN_OP, rule5, fail)
rule5:
  // <assign_op> ::= /=
  TERMINAL_CONTENT_NF("/=", ASSIGN_OP, rule6, fail)
rule6:
  // <assign_op> ::= %=
  TERMINAL_CONTENT_NF("%=", ASSIGN_OP, rule7, fail)
rule7:
  // <assign_op> ::= <<=
  TERMINAL_CONTENT_NF("<<=", ASSIGN_OP, rule8, fail)
rule8:
  // <assign_op> ::= >>=
  TERMINAL_CONTENT_NF(">>=", ASSIGN_OP, rule9, fail)
rule9:
  // <assign_op> ::= &=
  TERMINAL_CONTENT_NF("&=", ASSIGN_OP, rule10, fail)
rule10:
  // <assign_op> ::= ^=
  TERMINAL_CONTENT_NF("^=", ASSIGN_OP, rule11, fail)
rule11:
  // <assign_op> ::= |=
  TERMINAL_CONTENT_NF("|=", ASSIGN_OP, fail, fail)
fail:
  TERMINAL_FAIL()
}

CSTNode* postfix_op(ListNode<Token>* &token) {
  ENTER_INFO(POSTFIX_OP)
rule1:
  // <postfix_op> ::= ++
  TERMINAL_CONTENT_NF("++", POSTFIX_OP, rule2, fail)
rule2:
  // <postfix_op> ::= --
  TERMINAL_CONTENT_NF("--", POSTFIX_OP, fail, fail)
fail:
  TERMINAL_FAIL()
}

CSTNode* if_stmt(ListNode<Token>* &token) {
  INIT(IF_STMT)
  ENTER_INFO(IF_STMT)
rule1:
  // <if_stmt> ::= if ( <expr> ) <stmt> else <stmt>
  NONTERMINAL_N(_if, fail)
  NONTERMINAL_N(_left_paren, fail)
  NONTERMINAL_N(expr, fail)
  NONTERMINAL_N(_right_paren, fail)
  NONTERMINAL_N(stmt, fail)
  NONTERMINAL_N(_else, rule2)
  NONTERMINAL_NS(stmt, fail, success)
rule2:
  // <if_stmt> ::= if ( <expr> ) <stmt>
  goto success;
success:
  SUCCESS()
fail:
  FAIL()
}

CSTNode* for_stmt(ListNode<Token>* &token) {
  INIT(FOR_STMT)
  ENTER_INFO(FOR_STMT)
rule1:
  // <for_stmt> ::= for ( <expr> ; <expr> ; <expr> ) <stmt>
  NONTERMINAL_N(_for, fail)
  NONTERMINAL_N(_left_paren, fail)
  NONTERMINAL_N(expr, fail)
  NONTERMINAL_N(_semicolon, fail)
  NONTERMINAL_N(expr, fail)
  NONTERMINAL_N(_semicolon, fail)
  NONTERMINAL_N(expr, fail)
  NONTERMINAL_N(_right_paren, fail)
  NONTERMINAL_NS(stmt, fail, success)
success:
  SUCCESS()
fail:
  FAIL()
}

CSTNode* jump_stmt(ListNode<Token>* &token) {
  INIT(JUMP_STMT)
  ENTER_INFO(JUMP_STMT)
rule1:
  // <jump_stmt> ::= break ;
  NONTERMINAL_N(_break, rule2)
  NONTERMINAL_NS(_semicolon, fail, success)
rule2:
  // <jump_stmt> ::= continue ;
  RESTORE()
  NONTERMINAL_N(_continue, fail)
  NONTERMINAL_NS(_semicolon, fail, success)
success:
  SUCCESS()
fail:
  FAIL()
}

CSTNode* return_stmt(ListNode<Token>* &token) {
  INIT(RETURN_STMT)
  ENTER_INFO(RETURN_STMT)
rule1:
  // <return_stmt> ::= return <expr> ;
  NONTERMINAL_N(_return, fail)
  NONTERMINAL_N(expr, rule2)
  NONTERMINAL_NS(_semicolon, fail, success)
rule2:
  // <return_stmt> ::= return ;
  NONTERMINAL_NS(_semicolon, fail, success)
success:
  SUCCESS()
fail:
  FAIL()
}

CSTNode* type(ListNode<Token>* &token) {
  INIT(TYPE)
  ENTER_INFO(TYPE)
rule1:
  // <type> ::= int *
  NONTERMINAL_N(_int, rule3)
  NONTERMINAL_NS(_asterisk, rule2, success)
rule2:
  // <type> ::= int
  goto success;
rule3:
  // <type> ::= char *
  RESTORE()
  NONTERMINAL_N(_char, rule5)
  NONTERMINAL_NS(_asterisk, rule4, success)
rule4:
  // <type> ::= char
  goto success;
rule5:
  // <type> ::= void *
  RESTORE()
  NONTERMINAL_N(_void, rule7)
  NONTERMINAL_NS(_asterisk, rule6, success)
rule6:
  // <type> ::= void
  goto success;
rule7:
  // <type> ::= struct <id> *
  RESTORE()
  NONTERMINAL_N(_struct, fail)
  NONTERMINAL_N(id, fail)
  NONTERMINAL_NS(_asterisk, rule8, success)
rule8:
  // <type> ::= struct <id>
  goto success;
success:
  SUCCESS()
fail:
  FAIL()
}

CSTNode* expr(ListNode<Token>* &token) {
  INIT(EXPR)
  ENTER_INFO(EXPR)
rule1:
  // <expr> ::= <cond_expr> <eq_op> <expr>
  NONTERMINAL_N(cond_expr, fail)
  NONTERMINAL_N(eq_op, rule2)
  NONTERMINAL_NS(expr, fail, success)
rule2:
  // <expr> ::= <cond_expr>
  goto success;
success:
  SUCCESS()
fail:
  FAIL()
}

CSTNode* eq_op(ListNode<Token>* &token) {
  ENTER_INFO(EQ_OP)
rule1:
  // <eq_op> ::= ==
  TERMINAL_CONTENT_NF("==", EQ_OP, rule2, fail)
rule2:
  // <eq_op> ::= !=
  TERMINAL_CONTENT_NF("!=", EQ_OP, fail, fail)
fail:
  TERMINAL_FAIL()
}

CSTNode* cond_expr(ListNode<Token>* &token) {
  INIT(COND_EXPR)
  ENTER_INFO(COND_EXPR)
rule1:
  // <cond_expr> ::= <shift_expr> <cond_op> <cond_expr>
  NONTERMINAL_N(shift_expr, fail)
  NONTERMINAL_N(cond_op, rule2)
  NONTERMINAL_NS(cond_expr, fail, success)
rule2:
  // <cond_expr> ::= <shift_expr>
  goto success;
success:
  SUCCESS()
fail:
  FAIL()
}

CSTNode* cond_op(ListNode<Token>* &token) {
  ENTER_INFO(COND_OP)
rule1:
  // <cond_op> ::= <
  TERMINAL_CONTENT_NF("<", COND_OP, rule2, fail)
rule2:
  // <cond_op> ::= >
  TERMINAL_CONTENT_NF(">", COND_OP, rule3, fail)
rule3:
  // <cond_op> ::= <=
  TERMINAL_CONTENT_NF("<=", COND_OP, rule4, fail)
rule4:
  // <cond_op> ::= >=
  TERMINAL_CONTENT_NF(">=", COND_OP, fail, fail)
fail:
  TERMINAL_FAIL()
}

CSTNode* shift_expr(ListNode<Token>* &token) {
  INIT(SHIFT_EXPR)
  ENTER_INFO(SHIFT_EXPR)
rule1:
  // <shift_expr> ::= <add_expr> <shift_op> <shift_expr>
  NONTERMINAL_N(add_expr, fail)
  NONTERMINAL_N(shift_op, rule2)
  NONTERMINAL_NS(shift_expr, fail, success)
rule2:
  // <shift_expr> ::= <add_expr>
  goto success;
success:
  SUCCESS()
fail:
  FAIL()
}

CSTNode* shift_op(ListNode<Token>* &token) {
  ENTER_INFO(SHIFT_OP)
rule1:
  // <shift_expr> ::= <<
  TERMINAL_CONTENT_NF("<<", SHIFT_OP, rule2, fail)
rule2:
  // <shift_expr> ::= >>
  TERMINAL_CONTENT_NF(">>", SHIFT_OP, fail, fail)
fail:
  TERMINAL_FAIL()
}

CSTNode* add_expr(ListNode<Token>* &token) {
  INIT(ADD_EXPR)
  ENTER_INFO(ADD_EXPR)
rule1:
  // <add_expr> ::= <mul_expr> <add_op> <add_expr>
  NONTERMINAL_N(mul_expr, fail)
  NONTERMINAL_N(add_op, rule2)
  NONTERMINAL_NS(add_expr, fail, success)
rule2:
  // <add_expr> ::= <mul_expr>
  goto success;
success:
  SUCCESS()
fail:
  FAIL()
}

CSTNode* add_op(ListNode<Token>* &token) {
  ENTER_INFO(ADD_OP)
rule1:
  // <add_expr> ::= +
  TERMINAL_CONTENT_NF("+", ADD_OP, rule2, fail)
rule2:
  // <add_expr> ::= -
  TERMINAL_CONTENT_NF("-", ADD_OP, fail, fail)
fail:
  TERMINAL_FAIL()
}

CSTNode* mul_expr(ListNode<Token>* &token) {
  INIT(MUL_EXPR)
  ENTER_INFO(MUL_EXPR)
rule1:
  // <mul_expr> ::= <unary_expr> <mul_op> <mul_expr>
  NONTERMINAL_N(unary_expr, fail)
  NONTERMINAL_N(mul_op, rule2)
  NONTERMINAL_NS(mul_expr, fail, success)
rule2:
  // <mul_expr> ::= <unary_expr>
  goto success;
success:
  SUCCESS()
fail:
  FAIL()
}

CSTNode* mul_op(ListNode<Token>* &token) {
  ENTER_INFO(MUL_OP)
rule1:
  // <mul_expr> ::= *
  TERMINAL_CONTENT_NF("*", MUL_OP, rule2, fail)
rule2:
  // <mul_expr> ::= /
  TERMINAL_CONTENT_NF("/", MUL_OP, rule3, fail)
rule3:
  // <mul_expr> ::= %
  TERMINAL_CONTENT_NF("%", MUL_OP, fail, fail)
fail:
  TERMINAL_FAIL()
}

CSTNode* unary_expr(ListNode<Token>* &token) {
  INIT(UNARY_EXPR)
  ENTER_INFO(UNARY_EXPR)
rule1:
  // <unary_expr> ::= <term>
  NONTERMINAL_NS(term, rule2, success)
rule2:
  // <unary_expr> ::= <unary_op> <unary_expr>
  RESTORE()
  NONTERMINAL_N(unary_op, fail)
  NONTERMINAL_NS(unary_expr, fail, success)
success:
  SUCCESS()
fail:
  FAIL()
}

CSTNode* unary_op(ListNode<Token>* &token) {
  ENTER_INFO(UNARY_OP)
rule1:
  // <unary_expr> ::= +
  TERMINAL_CONTENT_NF("+", UNARY_OP, rule2, fail)
rule2:
  // <unary_expr> ::= -
  TERMINAL_CONTENT_NF("-", UNARY_OP, rule3, fail)
rule3:
  // <unary_expr> ::= !
  TERMINAL_CONTENT_NF("!", UNARY_OP, rule4, fail)
rule4:
  // <unary_expr> ::= ~
  TERMINAL_CONTENT_NF("~", UNARY_OP, fail, fail)
fail:
  TERMINAL_FAIL()
}

CSTNode* term(ListNode<Token>* &token) {
  INIT(TERM)
  ENTER_INFO(TERM)
rule1:
  // <term> ::= <keyword_func> ( <para_list> )
  NONTERMINAL_N(keyword_func, rule2)
  NONTERMINAL_N(_left_paren, fail)
  NONTERMINAL_N(para_list, fail)
  NONTERMINAL_NS(_right_paren, fail, success)
rule2:
  // <term> ::= <lvalue> ( <para_list> )
  RESTORE()
  NONTERMINAL_N(lvalue, rule6)
  NONTERMINAL_N(_left_paren, rule3)
  NONTERMINAL_N(para_list, fail)
  NONTERMINAL_NS(_right_paren, fail, success)
rule3:
  // <term> ::= <lvalue> <assign_op> <expr>
  NONTERMINAL_N(assign_op, rule4)
  NONTERMINAL_NS(expr, fail, success)
rule4:
  // <term> ::= <lvalue> <postfix_op>
  NONTERMINAL_NS(postfix_op, rule5, success)
rule5:
  // <term> ::= <lvalue>
  goto success;
rule6:
  // <term> ::= <number>
  RESTORE()
  NONTERMINAL_NS(number, rule7, success)
rule7:
  // <term> ::= <char_lit>
  RESTORE()
  NONTERMINAL_NS(char_lit, rule8, success)
rule8:
  // <term> ::= <string_lit>
  RESTORE()
  NONTERMINAL_NS(string_lit, rule9, success)
rule9:
  // <term> ::= ( <expr> )
  RESTORE()
  NONTERMINAL_N(_left_paren, fail)
  NONTERMINAL_N(expr, fail)
  NONTERMINAL_NS(_right_paren, fail, success)
success:
  SUCCESS()
fail:
  FAIL()
}

CSTNode* keyword_func(ListNode<Token>* &token) {
  ENTER_INFO(KEYWORD_FUNC)
rule1:
  // <keyword_func> ::= printf
  TERMINAL_CONTENT_NF("printf", KEYWORD_FUNC, rule2, fail)
rule2:
  // <keyword_func> ::= scanf
  TERMINAL_CONTENT_NF("scanf", KEYWORD_FUNC, rule3, fail)
rule3:
  // <keyword_func> ::= open
  TERMINAL_CONTENT_NF("open", KEYWORD_FUNC, rule4, fail)
rule4:
  // <keyword_func> ::= close
  TERMINAL_CONTENT_NF("close", KEYWORD_FUNC, rule5, fail)
rule5:
  // <keyword_func> ::= read
  TERMINAL_CONTENT_NF("read", KEYWORD_FUNC, rule6, fail)
rule6:
  // <keyword_func> ::= write
  TERMINAL_CONTENT_NF("write", KEYWORD_FUNC, rule7, fail)
rule7:
  // <keyword_func> ::= malloc
  TERMINAL_CONTENT_NF("malloc", KEYWORD_FUNC, rule8, fail)
rule8:
  // <keyword_func> ::= free
  TERMINAL_CONTENT_NF("free", KEYWORD_FUNC, fail, fail)
fail:
  TERMINAL_FAIL()
}

CSTNode* lvalue(ListNode<Token>* &token) {
  INIT(LVALUE)
  ENTER_INFO(LVALUE)
rule1:
  // <lvalue> ::= <entity>  [ <expr> ]
  NONTERMINAL_N(entity, rule3)
  NONTERMINAL_N(_left_bracket, rule2)
  NONTERMINAL_N(expr, fail)
  NONTERMINAL_NS(_right_bracket, fail, success)
rule2:
  // <lvalue> ::= <entity>
  goto success;
rule3:
  // <lvalue> ::= * <lvalue>
  RESTORE()
  NONTERMINAL_N(_asterisk, rule4)
  NONTERMINAL_NS(lvalue, fail, success)
rule4:
  // <lvalue> ::= & <lvalue>
  RESTORE()
  NONTERMINAL_N(_ampersand, rule5)
  NONTERMINAL_NS(lvalue, fail, success)
rule5:
  // <lvalue> ::= ( <lvalue> )
  RESTORE()
  NONTERMINAL_N(_left_paren, fail)
  NONTERMINAL_N(lvalue, fail)
  NONTERMINAL_NS(_right_paren, fail, success)
success:
  SUCCESS()
fail:
  FAIL()
}

CSTNode* entity(ListNode<Token>* &token) {
  INIT(ENTITY)
  ENTER_INFO(ENTITY)
rule1:
  // <entity> ::= <id> . <entity>
  NONTERMINAL_N(id, fail)
  NONTERMINAL_N(_dot, rule2)
  NONTERMINAL_NS(entity, fail, success)
rule2:
  // <entity> ::= <id> -> <entity>
  NONTERMINAL_N(_arrow, rule3)
  NONTERMINAL_NS(entity, fail, success)
rule3:
  // <entity> ::= <id>
  goto success;
success:
  SUCCESS()
fail:
  FAIL()
}

CSTNode* para_list(ListNode<Token>* &token) {
  INIT(PARA_LIST)
  ENTER_INFO(PARA_LIST)
rule1:
  // <para_list> ::= <expr> , <para_list>
  NONTERMINAL_N(expr, fail)
  NONTERMINAL_N(_comma, rule2)
  NONTERMINAL_NS(para_list, fail, success)
rule2:
  // <para_list> ::= <expr>
  goto success;
success:
  SUCCESS()
fail:
  FAIL()
}

CSTNode* id(ListNode<Token>* &token) {
  ENTER_INFO(ID)
  TERMINAL_TYPE_NF(ID, ID, fail, fail)
fail:
  TERMINAL_FAIL()
}

CSTNode* number(ListNode<Token>* &token) {
  ENTER_INFO(NUMBER)
  TERMINAL_TYPE_NF(NUMBER, NUMBER, fail, fail)
fail:
  TERMINAL_FAIL()
}

CSTNode* char_lit(ListNode<Token>* &token) {
  ENTER_INFO(CHAR_LIT)
  TERMINAL_TYPE_NF(CHAR_LIT, CHAR_LIT, fail, fail)
fail:
  TERMINAL_FAIL()
}

CSTNode* string_lit(ListNode<Token>* &token) {
  ENTER_INFO(STRING_LIT)
  TERMINAL_TYPE_NF(STRING_LIT, STRING_LIT, fail, fail)
fail:
  TERMINAL_FAIL()
}

CSTNode* _left_paren(ListNode<Token>* &token) {
  ENTER_INFO(_LEFT_PAREN)
  TERMINAL_CONTENT_NF("(", _LEFT_PAREN, fail, fail)
fail:
  TERMINAL_FAIL()
}

CSTNode* _right_paren(ListNode<Token>* &token) {
  ENTER_INFO(_RIGHT_PAREN)
  TERMINAL_CONTENT_NF(")", _RIGHT_PAREN, fail, fail)
fail:
  TERMINAL_FAIL()
}

CSTNode* _semicolon(ListNode<Token>* &token) {
  ENTER_INFO(_SEMICOLON)
  TERMINAL_CONTENT_NO_CHECK_N(";", _SEMICOLON, fail)
fail:
  TERMINAL_FAIL()
}

CSTNode* _equals(ListNode<Token>* &token) {
  ENTER_INFO(_EQUALS)
  TERMINAL_CONTENT_NF("=", _EQUALS, fail, fail)
fail:
  TERMINAL_FAIL()
}

CSTNode* _struct(ListNode<Token>* &token) {
  ENTER_INFO(_STRUCT)
  TERMINAL_CONTENT_NF("struct", _STRUCT, fail, fail)
fail:
  TERMINAL_FAIL()
}

CSTNode* _enum(ListNode<Token>* &token) {
  ENTER_INFO(_ENUM)
  TERMINAL_CONTENT_NF("enum", _ENUM, fail, fail)
fail:
  TERMINAL_FAIL()
}

CSTNode* _left_brace(ListNode<Token>* &token) {
  ENTER_INFO(_LEFT_BRACE)
  TERMINAL_CONTENT_NF("{", _LEFT_BRACE, fail, fail)
fail:
  TERMINAL_FAIL()
}

CSTNode* _right_brace(ListNode<Token>* &token) {
  ENTER_INFO(_RIGHT_BRACE)
  TERMINAL_CONTENT_NO_CHECK_N("}", _RIGHT_BRACE, fail)
fail:
  TERMINAL_FAIL()
}

CSTNode* _comma(ListNode<Token>* &token) {
  ENTER_INFO(_COMMA)
  TERMINAL_CONTENT_NF(",", _COMMA, fail, fail)
fail:
  TERMINAL_FAIL()
}

CSTNode* _epsilon(ListNode<Token>* &token) {
  ENTER_INFO(_EPSILON)
  return new CSTNode(_EPSILON, "");
}

CSTNode* _if(ListNode<Token>* &token) {
  ENTER_INFO(_IF)
  TERMINAL_CONTENT_NF("if", _IF, fail, fail)
fail:
  TERMINAL_FAIL()
}

CSTNode* _else(ListNode<Token>* &token) {
  ENTER_INFO(_ELSE)
  TERMINAL_CONTENT_NF("else", _ELSE, fail, fail)
fail:
  TERMINAL_FAIL()
}

CSTNode* _for(ListNode<Token>* &token) {
  ENTER_INFO(_FOR)
  TERMINAL_CONTENT_NF("for", _FOR, fail, fail)
fail:
  TERMINAL_FAIL()
}

CSTNode* _break(ListNode<Token>* &token) {
  ENTER_INFO(_BREAK)
  TERMINAL_CONTENT_NF("break", _BREAK, fail, fail)
fail:
  TERMINAL_FAIL()
}

CSTNode* _continue(ListNode<Token>* &token) {
  ENTER_INFO(_CONTINUE)
  TERMINAL_CONTENT_NF("continue", _CONTINUE, fail, fail)
fail:
  TERMINAL_FAIL()
}

CSTNode* _return(ListNode<Token>* &token) {
  ENTER_INFO(_RETURN)
  TERMINAL_CONTENT_NF("return", _RETURN, fail, fail)
fail:
  TERMINAL_FAIL()
}

CSTNode* _int(ListNode<Token>* &token) {
  ENTER_INFO(_INT)
  TERMINAL_CONTENT_NF("int", _INT, fail, fail)
fail:
  TERMINAL_FAIL()
}

CSTNode* _char(ListNode<Token>* &token) {
  ENTER_INFO(_CHAR)
  TERMINAL_CONTENT_NF("char", _CHAR, fail, fail)
fail:
  TERMINAL_FAIL()
}

CSTNode* _void(ListNode<Token>* &token) {
  ENTER_INFO(_VOID)
  TERMINAL_CONTENT_NF("void", _VOID, fail, fail)
fail:
  TERMINAL_FAIL()
}

CSTNode* _asterisk(ListNode<Token>* &token) {
  ENTER_INFO(_ASTERISK)
  TERMINAL_CONTENT_NF("*", _ASTERISK, fail, fail)
fail:
  TERMINAL_FAIL()
}

CSTNode* _ampersand(ListNode<Token>* &token) {
  ENTER_INFO(_AMPERSAND)
  TERMINAL_CONTENT_NF("&", _AMPERSAND, fail, fail)
fail:
  TERMINAL_FAIL()
}

CSTNode* _left_bracket(ListNode<Token>* &token) {
  ENTER_INFO(_LEFT_BRACKET)
  TERMINAL_CONTENT_NF("[", _LEFT_BRACKET, fail, fail)
fail:
  TERMINAL_FAIL()
}

CSTNode* _right_bracket(ListNode<Token>* &token) {
  ENTER_INFO(_RIGHT_BRACKET)
  TERMINAL_CONTENT_NF("]", _RIGHT_BRACKET, fail, fail)
fail:
  TERMINAL_FAIL()
}

CSTNode* _dot(ListNode<Token>* &token) {
  ENTER_INFO(_DOT)
  TERMINAL_CONTENT_NF(".", _DOT, fail, fail)
fail:
  TERMINAL_FAIL()
}

CSTNode* _arrow(ListNode<Token>* &token) {
  ENTER_INFO(_ARROW)
  TERMINAL_CONTENT_NF("->", _ARROW, fail, fail)
fail:
  TERMINAL_FAIL()
}

bool token_forward(ListNode<Token>* &token) {
  if (token == nullptr) {
    return false;
  }
  error_token = token;
  token = token->next;
  return true;
}

bool token_backward(ListNode<Token>* &token) {
  if (token == nullptr) {
    return false;
  }
  token = token->prev;
  return true;
}

CSTNode* build_CST(List<Token>* list) {
  // roughly check, needs better implementation in the future
  if (list == nullptr || list->head == nullptr) {
    std::fprintf(stderr, "AST Error: AST build failed: list is empty\n");
    exit(1);
  }

  ListNode<Token>* token = list->head;
  CSTNode* root = nullptr;

  if ((root = prog(token)) == nullptr || token != nullptr) {
    std::fprintf(
      stderr,
      "AST Error: AST build failed: syntax error near line %d, colunm %d\n",
      error_token->data->line,
      error_token->data->column
    );
    exit(1);
  }

  return root;
}

void graft_CST(CSTNode* node) {
  // general cases:
  //
  // +- <rule1>
  //    +- <rule2>                       +- <rule1>
  //       +- <rule3>   => graft_CST() =>   +- <rule4>
  //          +- <rule4>                    +- <rule5>
  //          +- <rule5>
  //
  // specical case: <expr> and <stmt>
  //   Since <expr> and <stmt> are important in further processing, I don't
  //   remove it while grafting.
  //
  // +- <rule1>                      +- <rule1>
  //    +- <expr>   => graft_CST() =>   +- <expr>
  //       + <rule2>                       +- <rule2>
  if (node->children.size() == 1 && node->children[0]->type != EXPR && node->children[0]->type != STMT) {
    CSTNode* temp = _graft_CST(node->children[0]);

    if (temp->children.size() == 0) {
      CSTNode* new_temp = new CSTNode(temp->type, temp->content);
      destruct_CST(node->children[0]);
      node->children[0] = new_temp;
    }
    else {
      for (int i = temp->children.size() - 1; i >= 0; i--) {
        node->children.insert(node->children.begin() + 1, temp->children[i]);
      }
      temp->children.clear();
      destruct_CST(node->children[0]);
      node->children.erase(node->children.begin());
    }
  }

  for (auto i : node->children) {
    graft_CST(i);
  }
}

void flatten_CST(CSTNode* node) {
  for (auto i : node->children) {
    flatten_CST(i);
  }

  // case 1: <prog>, <mem_decl_list>, and <stmt_list>
  //
  // +- <prog>
  //    +- <def>                           +- <prog>
  //    +- <prog>       => flatten_CST() =>   +- <def>
  //       +- <decl>                          +- <decl>
  //       +- <prog>
  //          +- <_epsilon>
  if (node->type == PROG ||
      node->type == MEM_DECL_LIST ||
      node->type == STMT_LIST) {
    if (node->children.size() > 1 &&
        (node->children.back()->type == PROG ||
         node->children.back()->type == MEM_DECL_LIST ||
         node->children.back()->type == STMT_LIST)) {
      CSTNode* temp = node->children.back();
      node->children.pop_back();
      for (auto &i : temp->children) {
        node->children.push_back(i);
      }
      delete temp;
    }
    else if (node->children.back()->type == PROG ||
             node->children.back()->type == MEM_DECL_LIST ||
             node->children.back()->type == STMT_LIST ||
             node->children.back()->type == _EPSILON){
      delete node->children[0];
      node->children.pop_back();
    }
  }
  // case 2: <enum_list>
  //
  // +- <enum_list>                      +- <enum_list>
  //    +- <id>                             +- <id>
  //    +- <comma>    => flatten_CST() =>   +- <comma>
  //    +- <enum_list>                      +- <id>
  //       +- <id>
  else if (node->type == ENUM_LIST) {
    if (node->children.size() > 1) {
      CSTNode* temp = node->children.back();
      node->children.pop_back();
      for (auto &i : temp->children) {
        node->children.push_back(i);
      }
      delete temp;
    }
  }
  // case 3: rules which are right-recursive (except <unary_expr>)
  //   condition 1:
  //
  //   +- <mul_expr>                          +- <mul_expr>
  //      +- <unary_expr>                        +- <unary_expr>
  //      +- <mul_op>      => flatten_CST() =>   +- <mul_op>
  //      +- <mul_expr>                          +- <unary_expr>
  //         +- <unary_expr>
  //
  //    condition 2:
  //      The program works fine without this condition, why did I write it?
  else if (node->type == EXPR ||
           node->type == COND_EXPR ||
           node->type == SHIFT_EXPR ||
           node->type == ADD_EXPR ||
           node->type == MUL_EXPR ||
           node->type == ARG_LIST ||
           node->type == PARA_LIST) {
    if (node->children.size() > 0 && node->children.back()->type == node->type) {
      CSTNode* temp = node->children.back();
      node->children.pop_back();
      for (auto &i : temp->children) {
        node->children.push_back(i);
      }
      delete temp;
    }
    // else if (node->children.size() > 0 &&
    //          node->children.back()->children.size() > 0 &&
    //          node->children[0]->type != UNARY_OP &&
    //          node->children[0]->type == node->children.back()->children[0]->type) {
    //   CSTNode* temp = node->children.back();
    //   node->children.pop_back();
    //   for (auto &i : temp->children) {
    //     node->children.push_back(i);
    //   }
    //   delete temp;
    // }
  }
  // case 4: expression-related nodes
  //   I think this case is no longer needed, but I leave it here just in case.
  // if (node->type == EXPR ||
  //     node->type == COND_EXPR ||
  //     node->type == SHIFT_EXPR ||
  //     node->type == ADD_EXPR ||
  //     node->type == MUL_EXPR ||
  //     node->type == UNARY_EXPR) {
  //   for (int i = 0; i < int(node->children.size()); i++) {
  //     if (node->children[i]->children.size() == 1 && (node->children[i]->children[0]->type == ID || node->children[i]->children[0]->type == NUMBER)) {
  //       CSTNode* temp = node->children[i];
  //       node->children[i] = temp->children[0];
  //       delete temp;
  //     }
  //   }
  // }
}

void fix_CST(CSTNode* node) {
  for (auto i : node->children) {
    fix_CST(i);
  }

  // case 1: <type>
  //    Let <type> node contains the message directly.
  //
  // +- <type>
  //    +- <_struct>  => fix_CST() => +- <type>
  //    +- <id>
  //    +- <_asterisk>
  if (node->type == TYPE) {
    if (node->children.size() == 1) {
      node->content = node->children[0]->content;
      delete node->children.back();
      node->children.pop_back();
    }
    else if (node->children.size() == 2){
      node->content = node->children[0]->content + " " + node->children[1]->content;
      delete node->children.back();
      node->children.pop_back();
      delete node->children.back();
      node->children.pop_back();
    }
    else {
      node->content = node->children[0]->content + " " + node->children[1]->content + " " + node->children[2]->content;
      delete node->children.back();
      node->children.pop_back();
      delete node->children.back();
      node->children.pop_back();
      delete node->children.back();
      node->children.pop_back();
    }
  }

  // case 2: expression-related nodes
  //    Since CST is generated, there's no need to distinguish them, so I rename them.
  if (node->type == COND_EXPR ||
      node->type == SHIFT_EXPR ||
      node->type == ADD_EXPR ||
      node->type == MUL_EXPR ||
      node->type == UNARY_EXPR) {
    node->type = EXPR;
  }

  // case 3: <_equals>
  if (node->type == _EQUALS) {
    node->type = ASSIGN_OP;
  }

  // case 4: <mul_expr>
  //   Okay, this is a little difficult to explain, but this is not needed now.
  // if (node->children.size() > 3 &&
  //     node->children[node->children.size() - 2]->type == UNARY_OP &&
  //     node->children[node->children.size() - 3]->type == MUL_OP) {
  //   CSTNode* temp = new CSTNode(EXPR, "");
  //   temp->children.push_back(node->children[node->children.size() - 2]);
  //   temp->children.push_back(node->children[node->children.size() - 1]);
  //   node->children[node->children.size() - 2] = temp;
  //   node->children.pop_back();
  // }

  // case 5: <number> and <id>
  //   This is used to fix some errors caused by the fourth case in
  //   flatten_CST(), since that case is no longer reachable, this case is
  //   naturally commented out.
  //
  // +- <number> => fix_CST() => +- <expr>
  //                                +- <number>
  // if (node->type == EXPR && node->children.size() > 1) {
  //   for (int i = 0; i < int(node->children.size()); i++) {
  //     if (node->children[i]->type == NUMBER || node->children[i]->type == ID) {
  //       CSTNode* temp = new CSTNode(EXPR, "");
  //       temp->children.push_back(node->children[i]);
  //       node->children[i] = temp;
  //     }
  //   }
  // }

  // case 6: <expr> and <lvalue>
  //    Note that fix_CST() is used after grift_CST(), and thus the following
  //    example is not wrong.
  //
  // +- <expr1>
  //    +- <_left_paren>
  //    +- <expr2>       => fix_CST() => +- <expr1>
  //       +- <number>                      +- <number>
  //    +- <_right_paren>
  if (node->type == EXPR || node->type == LVALUE) {
    if (node->children[0]->type == _LEFT_PAREN &&
        node->children[1]->type == node->type &&
        node->children[2]->type == _RIGHT_PAREN) {
      CSTNode* temp = node->children[1];
      delete node->children[0];
      delete node->children[2];
      node->children.clear();
      node->children = temp->children;
      delete temp;
    }
  }
}

void trim_CST(CSTNode* node) {
  // remove unnecessary node
  for (int i = node->children.size() - 1; i >= 0; i--) {
    CSTNode* temp = node->children[i];
    if (temp->type == _LEFT_PAREN ||
        temp->type == _RIGHT_PAREN ||
        // temp->type == _LEFT_BRACE ||
        // temp->type == _RIGHT_BRACE ||
        // temp->type == _LEFT_BRACKET ||
        // temp->type == _RIGHT_BRACKET ||
        temp->type == _COMMA ||
        temp->type == _SEMICOLON) {
      delete node->children[i];
      node->children.erase(node->children.begin() + i);
    }
  }
  for (auto i : node->children) {
    trim_CST(i);
  }
}

CSTNode* _graft_CST(CSTNode* node) {
  if (node->children.size() == 1) {
    return _graft_CST(node->children[0]);
  }
  else {
    return node;
  }
}

void destruct_CST(CSTNode* node) {
  for (auto &i : node->children) {
    destruct_CST(i);
  }
  delete node;
}

void display_CST(CSTNode* node) {
  _display_CST(node, "", true);
}

void _display_CST(CSTNode* node, std::string prefix, bool is_the_last) {
  // modify from: https://stackoverflow.com/a/8567550
  switch (node->type) {
    case TOKEN: {
      std::cout << prefix << "+- <token> ==> " << node->content << std::endl;
      break;
    }
    case PROG: {
      std::cout << prefix << "+- <prog>" << std::endl;
      break;
    }
    case DECL: {
      std::cout << prefix << "+- <decl>" << std::endl;
      break;
    }
    case ARG_LIST: {
      std::cout << prefix << "+- <arg_list>" << std::endl;
      break;
    }
    case ENUM_LIST: {
      std::cout << prefix << "+- <enum_list>" << std::endl;
      break;
    }
    case DEF: {
      std::cout << prefix << "+- <def>" << std::endl;
      break;
    }
    case MEM_DECL_LIST: {
      std::cout << prefix << "+- <mem_decl_list>" << std::endl;
      break;
    }
    case STMT_LIST: {
      std::cout << prefix << "+- <stmt_list>" << std::endl;
      break;
    }
    case STMT: {
      std::cout << prefix << "+- <stmt>" << std::endl;
      break;
    }
    case SIMPLE: {
      std::cout << prefix << "+- <simple>" << std::endl;
      break;
    }
    case ASSIGN_OP: {
      std::cout << prefix << "+- <assign_op> ==> " << node->content << std::endl;
      break;
    }
    case POSTFIX_OP: {
      std::cout << prefix << "+- <postfix_op> ==> " << node->content << std::endl;
      break;
    }
    case IF_STMT: {
      std::cout << prefix << "+- <if_stmt>" << std::endl;
      break;
    }
    case FOR_STMT: {
      std::cout << prefix << "+- <for_stmt>" << std::endl;
      break;
    }
    case JUMP_STMT: {
      std::cout << prefix << "+- <jump_stmt>" << std::endl;
      break;
    }
    case RETURN_STMT: {
      std::cout << prefix << "+- <return_stmt>" << std::endl;
      break;
    }
    case TYPE: {
      if (node->content != "") {
        std::cout << prefix << "+- <type> ==> " << node->content << std::endl;
      }
      else {
        std::cout << prefix << "+- <type>" << std::endl;
      }
      break;
    }
    case EXPR: {
      std::cout << prefix << "+- <expr>" << std::endl;
      break;
    }
    case EQ_OP: {
      std::cout << prefix << "+- <eq_op> ==> " << node->content << std::endl;
      break;
    }
    case COND_EXPR: {
      std::cout << prefix << "+- <cond_expr>" << std::endl;
      break;
    }
    case COND_OP: {
      std::cout << prefix << "+- <cond_op> ==> " << node->content << std::endl;
      break;
    }
    case SHIFT_EXPR: {
      std::cout << prefix << "+- <shift_expr>" << std::endl;
      break;
    }
    case SHIFT_OP: {
      std::cout << prefix << "+- <shift_op> ==> " << node->content << std::endl;
      break;
    }
    case ADD_EXPR: {
      std::cout << prefix << "+- <add_expr>" << std::endl;
      break;
    }
    case ADD_OP: {
      std::cout << prefix << "+- <add_op> ==> " << node->content << std::endl;
      break;
    }
    case MUL_EXPR: {
      std::cout << prefix << "+- <mul_expr>" << std::endl;
      break;
    }
    case MUL_OP: {
      std::cout << prefix << "+- <mul_op> ==> " << node->content << std::endl;
      break;
    }
    case UNARY_EXPR: {
      std::cout << prefix << "+- <unary_expr>" << std::endl;
      break;
    }
    case UNARY_OP: {
      std::cout << prefix << "+- <unary_op> ==> " << node->content << std::endl;
      break;
    }
    case TERM: {
      std::cout << prefix << "+- <term>" << std::endl;
      break;
    }
    case KEYWORD_FUNC: {
      std::cout << prefix << "+- <keyword_func> ==> " << node->content << std::endl;
      break;
    }
    case LVALUE: {
      std::cout << prefix << "+- <lvalue>" << std::endl;
      break;
    }
    case ENTITY: {
      std::cout << prefix << "+- <entity>" << std::endl;
      break;
    }
    case PARA_LIST: {
      std::cout << prefix << "+- <para_list>" << std::endl;
      break;
    }
    case ID: {
      std::cout << prefix << "+- <id> ==> " << node->content << std::endl;
      break;
    }
    case NUMBER: {
      std::cout << prefix << "+- <number> ==> " << node->content << std::endl;
      break;
    }
    case CHAR_LIT: {
      std::cout << prefix << "+- <char_lit> ==> " << node->content << std::endl;
      break;
    }
    case STRING_LIT: {
      std::cout << prefix << "+- <string_lit> ==> " << node->content << std::endl;
      break;
    }
    case _LEFT_PAREN: {
      std::cout << prefix << "+- <_left_paren> ==> " << node->content << std::endl;
      break;
    }
    case _RIGHT_PAREN: {
      std::cout << prefix << "+- <_right_paren> ==> " << node->content << std::endl;
      break;
    }
    case _SEMICOLON: {
      std::cout << prefix << "+- <_semicolon> ==> " << node->content << std::endl;
      break;
    }
    case _EQUALS: {
      std::cout << prefix << "+- <_equals> ==> " << node->content << std::endl;
      break;
    }
    case _STRUCT: {
      std::cout << prefix << "+- <_struct> ==> " << node->content << std::endl;
      break;
    }
    case _ENUM: {
      std::cout << prefix << "+- <_enum> ==> " << node->content << std::endl;
      break;
    }
    case _LEFT_BRACE: {
      std::cout << prefix << "+- <_left_brace> ==> " << node->content << std::endl;
      break;
    }
    case _RIGHT_BRACE: {
      std::cout << prefix << "+- <_right_brace> ==> " << node->content << std::endl;
      break;
    }
    case _COMMA: {
      std::cout << prefix << "+- <_comma> ==> " << node->content << std::endl;
      break;
    }
    case _EPSILON: {
      std::cout << prefix << "+- <_epsilon> ==> " << node->content << std::endl;
      break;
    }
    case _IF: {
      std::cout << prefix << "+- <_if> ==> " << node->content << std::endl;
      break;
    }
    case _ELSE: {
      std::cout << prefix << "+- <_else> ==> " << node->content << std::endl;
      break;
    }
    case _FOR: {
      std::cout << prefix << "+- <_for> ==> " << node->content << std::endl;
      break;
    }
    case _BREAK: {
      std::cout << prefix << "+- <_break> ==> " << node->content << std::endl;
      break;
    }
    case _CONTINUE: {
      std::cout << prefix << "+- <_continue> ==> " << node->content << std::endl;
      break;
    }
    case _RETURN: {
      std::cout << prefix << "+- <_return> ==> " << node->content << std::endl;
      break;
    }
    case _INT: {
      std::cout << prefix << "+- <_int> ==> " << node->content << std::endl;
      break;
    }
    case _CHAR: {
      std::cout << prefix << "+- <_char> ==> " << node->content << std::endl;
      break;
    }
    case _VOID: {
      std::cout << prefix << "+- <_void> ==> " << node->content << std::endl;
      break;
    }
    case _ASTERISK: {
      std::cout << prefix << "+- <_asterisk> ==> " << node->content << std::endl;
      break;
    }
    case _AMPERSAND: {
      std::cout << prefix << "+- <_ampersand> ==> " << node->content << std::endl;
      break;
    }
    case _LEFT_BRACKET: {
      std::cout << prefix << "+- <_left_bracket> ==> " << node->content << std::endl;
      break;
    }
    case _RIGHT_BRACKET: {
      std::cout << prefix << "+- <_right_bracket> ==> " << node->content << std::endl;
      break;
    }
    case _DOT: {
      std::cout << prefix << "+- <_dot> ==> " << node->content << std::endl;
      break;
    }
    case _ARROW: {
      std::cout << prefix << "+- <_arrow> ==> " << node->content << std::endl;
      break;
    }
    default: {
      std::cout << prefix << "+- <unknown>" << std::endl;
    }
  }
  prefix += is_the_last ? "   " : "|  ";
  int length = node->children.size();
  for (int i = 0; i < length; i++) {
    _display_CST(node->children[i], prefix, i == length-1);
  }
}
