#include <iostream>
#include "AST.h"
#include "List.h"
#include "Token.h"

#define DEBUG 0

ASTNode::ASTNode(int _type, std::string _content) {
  type = _type;
  content = _content;
}

ASTNode* build_AST(List<Token>* list) {
  // roughly check, needs better implementation in the future
  if (list == nullptr || list->head == nullptr) {
    std::fprintf(stderr, "AST Error: AST build failed: list is empty\n");
    exit(1);
  }

  ListNode<Token>* token = list->head;
  ASTNode* root = nullptr;

  if ((root = prog(token)) == nullptr || token != nullptr) {
    std::fprintf(stderr, "AST Error: AST build failed: syntax error\n");
    exit(1);
  }

  return root;
}

ASTNode* prog(ListNode<Token>* &token) {
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

ASTNode* decl(ListNode<Token>* &token) {
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

ASTNode* arg_list(ListNode<Token>* &token) {
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

ASTNode* enum_list(ListNode<Token>* &token) {
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

ASTNode* def(ListNode<Token>* &token) {
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

ASTNode* mem_decl_list(ListNode<Token>* &token) {
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

ASTNode* stmt_list(ListNode<Token>* &token) {
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

ASTNode* stmt(ListNode<Token>* &token) {
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

ASTNode* simple(ListNode<Token>* &token) {
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

ASTNode* assign_op(ListNode<Token>* &token) {
  ENTER_INFO(ASSIGN_OP)
rule1:
  // <assign_op> ::= =
  TERMINAL_CONTENT_NF("=", rule2, fail)
rule2:
  // <assign_op> ::= +=
  TERMINAL_CONTENT_NF("+=", rule3, fail)
rule3:
  // <assign_op> ::= -=
  TERMINAL_CONTENT_NF("-=", rule4, fail)
rule4:
  // <assign_op> ::= *=
  TERMINAL_CONTENT_NF("*=", rule5, fail)
rule5:
  // <assign_op> ::= /=
  TERMINAL_CONTENT_NF("/=", rule6, fail)
rule6:
  // <assign_op> ::= %=
  TERMINAL_CONTENT_NF("%=", rule7, fail)
rule7:
  // <assign_op> ::= <<=
  TERMINAL_CONTENT_NF("<<=", rule8, fail)
rule8:
  // <assign_op> ::= >>=
  TERMINAL_CONTENT_NF(">>=", rule9, fail)
rule9:
  // <assign_op> ::= &=
  TERMINAL_CONTENT_NF("&=", rule10, fail)
rule10:
  // <assign_op> ::= ^=
  TERMINAL_CONTENT_NF("^=", rule11, fail)
rule11:
  // <assign_op> ::= |=
  TERMINAL_CONTENT_NF("|=", fail, fail)
fail:
  TERMINAL_FAIL()
}

ASTNode* postfix_op(ListNode<Token>* &token) {
  ENTER_INFO(POSTFIX_OP)
rule1:
  // <postfix_op> ::= ++
  TERMINAL_CONTENT_NF("++", rule2, fail)
rule2:
  // <postfix_op> ::= --
  TERMINAL_CONTENT_NF("--", fail, fail)
fail:
  TERMINAL_FAIL()
}

ASTNode* if_stmt(ListNode<Token>* &token) {
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

ASTNode* for_stmt(ListNode<Token>* &token) {
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

ASTNode* jump_stmt(ListNode<Token>* &token) {
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

ASTNode* return_stmt(ListNode<Token>* &token) {
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

ASTNode* type(ListNode<Token>* &token) {
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

ASTNode* expr(ListNode<Token>* &token) {
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

ASTNode* eq_op(ListNode<Token>* &token) {
  ENTER_INFO(EQ_OP)
rule1:
  // <eq_op> ::= ==
  TERMINAL_CONTENT_NF("==", rule2, fail)
rule2:
  // <eq_op> ::= !=
  TERMINAL_CONTENT_NF("!=", fail, fail)
fail:
  TERMINAL_FAIL()
}

ASTNode* cond_expr(ListNode<Token>* &token) {
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

ASTNode* cond_op(ListNode<Token>* &token) {
  ENTER_INFO(COND_OP)
rule1:
  // <cond_op> ::= <
  TERMINAL_CONTENT_NF("<", rule2, fail)
rule2:
  // <cond_op> ::= >
  TERMINAL_CONTENT_NF(">", rule3, fail)
rule3:
  // <cond_op> ::= <=
  TERMINAL_CONTENT_NF("<=", rule4, fail)
rule4:
  // <cond_op> ::= >=
  TERMINAL_CONTENT_NF(">=", fail, fail)
fail:
  TERMINAL_FAIL()
}

ASTNode* shift_expr(ListNode<Token>* &token) {
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

ASTNode* shift_op(ListNode<Token>* &token) {
  ENTER_INFO(SHIFT_OP)
rule1:
  // <shift_expr> ::= <<
  TERMINAL_CONTENT_NF("<<", rule2, fail)
rule2:
  // <shift_expr> ::= >>
  TERMINAL_CONTENT_NF(">>", fail, fail)
fail:
  TERMINAL_FAIL()
}

ASTNode* add_expr(ListNode<Token>* &token) {
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

ASTNode* add_op(ListNode<Token>* &token) {
  ENTER_INFO(ADD_OP)
rule1:
  // <add_expr> ::= +
  TERMINAL_CONTENT_NF("+", rule2, fail)
rule2:
  // <add_expr> ::= -
  TERMINAL_CONTENT_NF("-", fail, fail)
fail:
  TERMINAL_FAIL()
}

ASTNode* mul_expr(ListNode<Token>* &token) {
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

ASTNode* mul_op(ListNode<Token>* &token) {
  ENTER_INFO(MUL_OP)
rule1:
  // <mul_expr> ::= *
  TERMINAL_CONTENT_NF("*", rule2, fail)
rule2:
  // <mul_expr> ::= /
  TERMINAL_CONTENT_NF("/", rule3, fail)
rule3:
  // <mul_expr> ::= %
  TERMINAL_CONTENT_NF("%", fail, fail)
fail:
  TERMINAL_FAIL()
}

ASTNode* unary_expr(ListNode<Token>* &token) {
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

ASTNode* unary_op(ListNode<Token>* &token) {
  ENTER_INFO(UNARY_OP)
rule1:
  // <unary_expr> ::= +
  TERMINAL_CONTENT_NF("+", rule2, fail)
rule2:
  // <unary_expr> ::= -
  TERMINAL_CONTENT_NF("-", rule3, fail)
rule3:
  // <unary_expr> ::= !
  TERMINAL_CONTENT_NF("!", rule4, fail)
rule4:
  // <unary_expr> ::= ~
  TERMINAL_CONTENT_NF("~", fail, fail)
fail:
  TERMINAL_FAIL()
}

ASTNode* term(ListNode<Token>* &token) {
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

ASTNode* keyword_func(ListNode<Token>* &token) {
  ENTER_INFO(KEYWORD_FUNC)
rule1:
  // <keyword_func> ::= printf
  TERMINAL_CONTENT_NF("printf", rule2, fail)
rule2:
  // <keyword_func> ::= scanf
  TERMINAL_CONTENT_NF("scanf", rule3, fail)
rule3:
  // <keyword_func> ::= open
  TERMINAL_CONTENT_NF("open", rule4, fail)
rule4:
  // <keyword_func> ::= close
  TERMINAL_CONTENT_NF("close", rule5, fail)
rule5:
  // <keyword_func> ::= read
  TERMINAL_CONTENT_NF("read", rule6, fail)
rule6:
  // <keyword_func> ::= write
  TERMINAL_CONTENT_NF("write", rule7, fail)
rule7:
  // <keyword_func> ::= malloc
  TERMINAL_CONTENT_NF("malloc", rule8, fail)
rule8:
  // <keyword_func> ::= free
  TERMINAL_CONTENT_NF("free", fail, fail)
fail:
  TERMINAL_FAIL()
}

ASTNode* lvalue(ListNode<Token>* &token) {
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

ASTNode* entity(ListNode<Token>* &token) {
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

ASTNode* para_list(ListNode<Token>* &token) {
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

ASTNode* id(ListNode<Token>* &token) {
  ENTER_INFO(ID)
  TERMINAL_TYPE_NF(ID, ID, fail, fail)
fail:
  TERMINAL_FAIL()
}

ASTNode* number(ListNode<Token>* &token) {
  ENTER_INFO(NUMBER)
  TERMINAL_TYPE_NF(NUMBER, NUMBER, fail, fail)
fail:
  TERMINAL_FAIL()
}

ASTNode* char_lit(ListNode<Token>* &token) {
  ENTER_INFO(CHAR_LIT)
  TERMINAL_TYPE_NF(CHAR_LIT, CHAR_LIT, fail, fail)
fail:
  TERMINAL_FAIL()
}

ASTNode* string_lit(ListNode<Token>* &token) {
  ENTER_INFO(STRING_LIT)
  TERMINAL_TYPE_NF(STRING_LIT, STRING_LIT, fail, fail)
fail:
  TERMINAL_FAIL()
}

ASTNode* _left_paren(ListNode<Token>* &token) {
  ENTER_INFO(_LEFT_PAREN)
  TERMINAL_CONTENT_NF("(", fail, fail)
fail:
  TERMINAL_FAIL()
}

ASTNode* _right_paren(ListNode<Token>* &token) {
  ENTER_INFO(_RIGHT_PAREN)
  TERMINAL_CONTENT_NF(")", fail, fail)
fail:
  TERMINAL_FAIL()
}

ASTNode* _semicolon(ListNode<Token>* &token) {
  ENTER_INFO(_SEMICOLON)
  TERMINAL_CONTENT_NO_CHECK_N(";", fail)
fail:
  TERMINAL_FAIL()
}

ASTNode* _equals(ListNode<Token>* &token) {
  ENTER_INFO(_EQUALS)
  TERMINAL_CONTENT_NF("=", fail, fail)
fail:
  TERMINAL_FAIL()
}

ASTNode* _struct(ListNode<Token>* &token) {
  ENTER_INFO(_STRUCT)
  TERMINAL_CONTENT_NF("struct", fail, fail)
fail:
  TERMINAL_FAIL()
}

ASTNode* _enum(ListNode<Token>* &token) {
  ENTER_INFO(_ENUM)
  TERMINAL_CONTENT_NF("enum", fail, fail)
fail:
  TERMINAL_FAIL()
}

ASTNode* _left_brace(ListNode<Token>* &token) {
  ENTER_INFO(_LEFT_BRACE)
  TERMINAL_CONTENT_NF("{", fail, fail)
fail:
  TERMINAL_FAIL()
}

ASTNode* _right_brace(ListNode<Token>* &token) {
  ENTER_INFO(_RIGHT_BRACE)
  TERMINAL_CONTENT_NO_CHECK_N("}", fail)
fail:
  TERMINAL_FAIL()
}

ASTNode* _comma(ListNode<Token>* &token) {
  ENTER_INFO(_COMMA)
  TERMINAL_CONTENT_NF(",", fail, fail)
fail:
  TERMINAL_FAIL()
}

ASTNode* _epsilon(ListNode<Token>* &token) {
  ENTER_INFO(_EPSILON)
  return new ASTNode(_EPSILON, "");
}

ASTNode* _if(ListNode<Token>* &token) {
  ENTER_INFO(_IF)
  TERMINAL_CONTENT_NF("if", fail, fail)
fail:
  TERMINAL_FAIL()
}

ASTNode* _else(ListNode<Token>* &token) {
  ENTER_INFO(_ELSE)
  TERMINAL_CONTENT_NF("else", fail, fail)
fail:
  TERMINAL_FAIL()
}

ASTNode* _for(ListNode<Token>* &token) {
  ENTER_INFO(_FOR)
  TERMINAL_CONTENT_NF("for", fail, fail)
fail:
  TERMINAL_FAIL()
}

ASTNode* _break(ListNode<Token>* &token) {
  ENTER_INFO(_BREAK)
  TERMINAL_CONTENT_NF("break", fail, fail)
fail:
  TERMINAL_FAIL()
}

ASTNode* _continue(ListNode<Token>* &token) {
  ENTER_INFO(_CONTINUE)
  TERMINAL_CONTENT_NF("continue", fail, fail)
fail:
  TERMINAL_FAIL()
}

ASTNode* _return(ListNode<Token>* &token) {
  ENTER_INFO(_RETURN)
  TERMINAL_CONTENT_NF("return", fail, fail)
fail:
  TERMINAL_FAIL()
}

ASTNode* _int(ListNode<Token>* &token) {
  ENTER_INFO(_INT)
  TERMINAL_CONTENT_NF("int", fail, fail)
fail:
  TERMINAL_FAIL()
}

ASTNode* _char(ListNode<Token>* &token) {
  ENTER_INFO(_CHAR)
  TERMINAL_CONTENT_NF("char", fail, fail)
fail:
  TERMINAL_FAIL()
}

ASTNode* _void(ListNode<Token>* &token) {
  ENTER_INFO(_VOID)
  TERMINAL_CONTENT_NF("void", fail, fail)
fail:
  TERMINAL_FAIL()
}

ASTNode* _asterisk(ListNode<Token>* &token) {
  ENTER_INFO(_ASTERISK)
  TERMINAL_CONTENT_NF("*", fail, fail)
fail:
  TERMINAL_FAIL()
}

ASTNode* _ampersand(ListNode<Token>* &token) {
  ENTER_INFO(_AMPERSAND)
  TERMINAL_CONTENT_NF("&", fail, fail)
fail:
  TERMINAL_FAIL()
}

ASTNode* _left_bracket(ListNode<Token>* &token) {
  ENTER_INFO(_LEFT_BRACKET)
  TERMINAL_CONTENT_NF("[", fail, fail)
fail:
  TERMINAL_FAIL()
}

ASTNode* _right_bracket(ListNode<Token>* &token) {
  ENTER_INFO(_RIGHT_BRACKET)
  TERMINAL_CONTENT_NF("]", fail, fail)
fail:
  TERMINAL_FAIL()
}

ASTNode* _dot(ListNode<Token>* &token) {
  ENTER_INFO(_DOT)
  TERMINAL_CONTENT_NF(".", fail, fail)
fail:
  TERMINAL_FAIL()
}

ASTNode* _arrow(ListNode<Token>* &token) {
  ENTER_INFO(_ARROW)
  TERMINAL_CONTENT_NF("->", fail, fail)
fail:
  TERMINAL_FAIL()
}

bool token_forward(ListNode<Token>* &token) {
  if (token == nullptr) {
    return false;
  }
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
