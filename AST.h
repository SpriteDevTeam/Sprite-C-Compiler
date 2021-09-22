#ifndef __AST_H__
#define __AST_H__

#include <string>
#include <vector>
#include "List.h"
#include "Token.h"

#define INIT(ast_type) \
  ASTNode* node = new ASTNode(ast_type, ""); \
  ASTNode* temp = nullptr; \
  ListNode<Token>* token_copy = token;

#define RESTORE() \
  for (auto &i : node->children) delete i; \
  node->children.clear(); \
  token = token_copy;

#define SUCCESS() \
  return node;

#define FAIL() \
  node->children.clear(); \
  delete node; \
  return nullptr;

#define NONTERMINAL_N(nonterminal, next_rule) \
  if ((temp = nonterminal(token)) == nullptr) goto next_rule; \
  else node->children.push_back(temp);

#define NONTERMINAL_NS(nonterminal, next_rule, success_rule) \
  if ((temp = nonterminal(token)) == nullptr) goto next_rule; \
  else { \
    node->children.push_back(temp); \
    goto success_rule; \
  }

#define TERMINAL_TYPE_NF(_type, ast_type, next_rule, fail_rule) \
  if (token->data->type == _type) { \
    std::string content = token->data->content; \
    if (token_forward(token)) \
      return new ASTNode(ast_type, content); \
    else goto fail_rule; \
  } \
  else goto next_rule;

#define TERMINAL_CONTENT_NF(terminal, next_rule, fail_rule) \
  if (token->data->content == terminal) { \
    std::string content = token->data->content; \
    if (token_forward(token)) \
      return new ASTNode(TOKEN, content); \
    else goto fail_rule; \
  } \
  else goto next_rule;

#define TERMINAL_CONTENT_NO_CHECK_N(terminal, next_rule) \
  if (token->data->content == terminal) { \
    std::string content = token->data->content; \
    token_forward(token); \
    return new ASTNode(TOKEN, content); \
  } \
  else goto next_rule;

#define ENTER_INFO(ast_type) \
  if (DEBUG) { \
    if (token) std::cerr << #ast_type << ": " << token->data->content << std::endl; \
  }

#define TERMINAL_FAIL() \
  return nullptr;

// <prog> ::= <decl>
//          | <def>
//          | <decl> <prog>
//          | <def> <prog>
//
// <decl> ::= <type> <id> ( ) ;
//          | <type> <id> ( <arg_list> ) ;
//          | <type> <id> ;
//          | <type> <id> = <expr> ;
//          | struct <id> ;
//          | enum { <enum_list> } ;
//
// <arg_list> ::= <type> <id>
//              | <type> <id> , <arg_list>
//
// <enum_list> ::= <id> = <number>
//               | <id>
//               | <id> = <number> , <enum_list>
//               | <id> , <enum_list>
//
// <def> ::= struct <id> { <mem_decl_list> } ;
//         | <type> <id> ( <arg_list> ) { <stmt_list> }
//         | <type> <id> ( ) { <stmt_list> }
//
// <mem_decl_list> ::= <type> <id> ; <mem_decl_list>
//                   | epsilon
//
// <stmt_list> ::= <stmt> <stmt_list>
//               | epsilon
//
// <stmt> ::= ;
//          | <simple>
//          | <if_stmt>
//          | <for_stmt>
//          | <jump_stmt>
//          | <return_stmt>
//          | { <stmt_list> }
//
// <simple> ::= <expr> ;
//            | <type> <id> = <expr> ;
//            | <type> <id> ;
//
// <assign_op> ::= =
//              | +=
//              | -=
//              | *=
//              | /=
//              | %=
//              | <<=
//              | >>=
//              | &=
//              | ^=
//              | |=
//
// <postfix_op> ::= ++
//                | --
//
// <if_stmt> ::= if ( <expr> ) <stmt> else <stmt>
//             | if ( <expr> ) <stmt>
//
// <for_stmt> ::= for ( <expr> ; <expr> ; <expr> ) <stmt>
//
// <jump_stmt> ::= break ;
//               | continue ;
//
// <return_stmt> ::= return <expr> ;
//                 | return ;
//
// <type> ::= int *
//          | int
//          | char *
//          | char
//          | void *
//          | void
//          | struct <id> *
//          | struct <id>
//
// <expr> ::= <cond_expr> <eq_op> <expr>
//          | <cond_expr>
//
// <eq_op> ::= ==
//           | !=
//
// <cond_expr> ::= <shift_expr> <cond_op> <cond_expr>
//               | <shift_expr>
//
// <cond_op> ::= <
//             | >
//             | <=
//             | >=
//
// <shift_expr> ::= <add_expr> <shift_op> <shift_expr>
//                | <add_expr>
//
// <shift_op> ::= <<
//              | >>
//
// <add_expr> ::= <mul_expr> <add_op> <add_expr>
//              | <mul_expr>
//
// <add_op> ::= +
//            | -
//
// <mul_expr> ::= <unary_expr> <mul_op> <mul_expr>
//              | <unary_expr>
//
// <mul_op> ::= *
//            | /
//            | %
//
// <unary_expr> ::= <term>
//                | <unary_op> <unary_expr>
//
// <unary_op> ::= +
//              | -
//              | !
//              | ~
//
// <term> ::= <keyword_func> ( <para_list> )
//          | <lvalue> ( <para_list> )
//          | <lvalue> <assign_op> <expr>
//          | <lvalue> <postfix_op>
//          | <lvalue>
//          | <number>
//          | <char_lit>
//          | <string_lit>
//          | ( <expr> )
//
// <keyword_func> ::= printf
//                  | scanf
//                  | open
//                  | close
//                  | read
//                  | write
//                  | malloc
//                  | free
//
// <lvalue> ::= <entity> [ <expr> ]
//            | <entity>
//            | * <lvalue>
//            | & <lvalue>
//            | ( <lvalue> )
//
// <entity> ::= <id> . <entity>
//            | <id> -> <entity>
//            | <id>
//
// <para_list> ::= <expr> , <para_list>
//               | <expr>
//
// 1. Any ASTs formed by the above rules will have all operators to be
//    right-associative. When traversing the AST, operators that have
//    left-associative should be interpreted carefully.
// 2. One important principle in implementing the above rules is that every
//    rule containing both nonterminals and terminals is converted to another
//    equivalent rule only containing nonterminals, all terminals in this kind
//    of rules are replaced by nonterminals which only contain themselves. The
//    new derived rules are showed below.
//
// <_left_paren> ::= (
// <_right_paren> ::= )
// <_semicolon> ::= ;
// <_equals> ::= =
// <_struct> ::= struct
// <_enum> ::= enum
// <_left_brace> ::= {
// <_right_brace> ::= }
// <_comma> ::= ,
// <_epsilon> ::= ε
// <_if> ::= if
// <_else> ::= else
// <_for> ::= for
// <_break> ::= break
// <_continue> ::= continue
// <_return> ::= return
// <_int> ::= int
// <_char> ::= char
// <_void> ::= void
// <_asterisk> ::= *
// <_ampersand> ::= &
// <_left_bracket> ::= [
// <_right_bracket> ::= ]
// <_dot> ::= .
// <_arrow> ::= ->

// value of assign_op, postfix, id, num, char_lit, string_lit are defined
// in Token.h
enum {
  TOKEN = 200,
  PROG,
  DECL,
  ARG_LIST,
  ENUM_LIST,
  DEF,
  MEM_DECL_LIST,
  STMT_LIST,
  STMT,
  SIMPLE,
  IF_STMT,
  FOR_STMT,
  JUMP_STMT,
  RETURN_STMT,
  TYPE,
  EXPR,
  EQ_OP,
  COND_EXPR,
  COND_OP,
  SHIFT_EXPR,
  SHIFT_OP,
  ADD_EXPR,
  ADD_OP,
  MUL_EXPR,
  MUL_OP,
  UNARY_EXPR,
  UNARY_OP,
  TERM,
  KEYWORD_FUNC,
  LVALUE,
  ENTITY,
  PARA_LIST,
  _LEFT_PAREN,
  _RIGHT_PAREN,
  _SEMICOLON,
  _EQUALS,
  _STRUCT,
  _ENUM,
  _LEFT_BRACE,
  _RIGHT_BRACE,
  _COMMA,
  _EPSILON,
  _IF,
  _ELSE,
  _FOR,
  _BREAK,
  _CONTINUE,
  _RETURN,
  _INT,
  _CHAR,
  _VOID,
  _ASTERISK,
  _AMPERSAND,
  _LEFT_BRACKET,
  _RIGHT_BRACKET,
  _DOT,
  _ARROW
};

struct ASTNode {
  int type;
  std::string content;
  std::vector<ASTNode*> children;
  ASTNode(int _type, std::string content);
};

ASTNode* build_AST(List<Token>* list);
void destruct_AST(ASTNode* node);

// AST functions
ASTNode* prog(ListNode<Token>* &token);
ASTNode* decl(ListNode<Token>* &token);
ASTNode* arg_list(ListNode<Token>* &token);
ASTNode* enum_list(ListNode<Token>* &token);
ASTNode* def(ListNode<Token>* &token);
ASTNode* mem_decl_list(ListNode<Token>* &token);
ASTNode* stmt_list(ListNode<Token>* &token);
ASTNode* stmt(ListNode<Token>* &token);
ASTNode* simple(ListNode<Token>* &token);
ASTNode* assign_op(ListNode<Token>* &token);
ASTNode* postfix_op(ListNode<Token>* &token);
ASTNode* if_stmt(ListNode<Token>* &token);
ASTNode* for_stmt(ListNode<Token>* &token);
ASTNode* jump_stmt(ListNode<Token>* &token);
ASTNode* return_stmt(ListNode<Token>* &token);
ASTNode* type(ListNode<Token>* &token);
ASTNode* expr(ListNode<Token>* &token);
ASTNode* eq_op(ListNode<Token>* &token);
ASTNode* cond_expr(ListNode<Token>* &token);
ASTNode* cond_op(ListNode<Token>* &token);
ASTNode* shift_expr(ListNode<Token>* &token);
ASTNode* shift_op(ListNode<Token>* &token);
ASTNode* add_expr(ListNode<Token>* &token);
ASTNode* add_op(ListNode<Token>* &token);
ASTNode* mul_expr(ListNode<Token>* &token);
ASTNode* mul_op(ListNode<Token>* &token);
ASTNode* unary_expr(ListNode<Token>* &token);
ASTNode* unary_op(ListNode<Token>* &token);
ASTNode* term(ListNode<Token>* &token);
ASTNode* keyword_func(ListNode<Token>* &token);
ASTNode* lvalue(ListNode<Token>* &token);
ASTNode* entity(ListNode<Token>* &token);
ASTNode* para_list(ListNode<Token>* &token);

ASTNode* id(ListNode<Token>* &token);
ASTNode* number(ListNode<Token>* &token);
ASTNode* char_lit(ListNode<Token>* &token);
ASTNode* string_lit(ListNode<Token>* &token);

ASTNode* _left_paren(ListNode<Token>* &token);
ASTNode* _right_paren(ListNode<Token>* &token);
ASTNode* _semicolon(ListNode<Token>* &token);
ASTNode* _equals(ListNode<Token>* &token);
ASTNode* _struct(ListNode<Token>* &token);
ASTNode* _enum(ListNode<Token>* &token);
ASTNode* _left_brace(ListNode<Token>* &token);
ASTNode* _right_brace(ListNode<Token>* &token);
ASTNode* _comma(ListNode<Token>* &token);
ASTNode* _epsilon(ListNode<Token>* &token);
ASTNode* _if(ListNode<Token>* &token);
ASTNode* _else(ListNode<Token>* &token);
ASTNode* _for(ListNode<Token>* &token);
ASTNode* _break(ListNode<Token>* &token);
ASTNode* _continue(ListNode<Token>* &token);
ASTNode* _return(ListNode<Token>* &token);
ASTNode* _int(ListNode<Token>* &token);
ASTNode* _char(ListNode<Token>* &token);
ASTNode* _void(ListNode<Token>* &token);
ASTNode* _asterisk(ListNode<Token>* &token);
ASTNode* _ampersand(ListNode<Token>* &token);
ASTNode* _left_bracket(ListNode<Token>* &token);
ASTNode* _right_bracket(ListNode<Token>* &token);
ASTNode* _dot(ListNode<Token>* &token);
ASTNode* _arrow(ListNode<Token>* &token);

// a <-> b <-> c  (next ->, prev <-)
// let p point to b
//     token_forward(p): p points to c
//     token_backward(p): p points to a
bool token_forward(ListNode<Token>* &token);
bool token_backward(ListNode<Token>* &token);

#endif
