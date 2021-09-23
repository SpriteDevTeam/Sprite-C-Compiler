#include "List.h"
#include "Parser.h"
#include "Token.h"

void parser(List<Token>* list) {

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
    std::fprintf(stderr, "AST Error: AST build failed: syntax error\n");
    exit(1);
  }

  return root;
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
  // modified from: https://stackoverflow.com/a/8567550
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
      std::cout << prefix << "+- <type>" << std::endl;
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
      std::cout << prefix << "+- <keyword_func>" << std::endl;
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
  }
  prefix += is_the_last ? "   " : "|  ";
  int length = node->children.size();
  for (int i = 0; i < length; i++) {
    _display_CST(node->children[i], prefix, i == length-1);
  }
}
