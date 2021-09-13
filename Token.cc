#include "Token.h"

Token::Token(int _type, std::string _content, int _line, int _column) {
  type = _type;
  content = _content;
  line = _line;
  column = _column;
}

void Token::display() {
  std::string message;

  switch (type) {
    case COMMENT_ERR: case CHAR_ERR: case STRING_ERR: case UNKNOWN_ERR: {
      std::printf("Omitted\n");
      return;
    }
    case TBD: {
      message = "to be determined";
      break;
    }
    case KEYWORD: {
      message = "keyword";
      break;
    }
    case ID: {
      message = "identifier";
      break;
    }
    case NUMBER: {
      message = "number";
      break;
    }
    case STRING_LIT: {
      message = "string literal";
      break;
    }
    case CHAR_LIT: {
      message = "char literal";
      break;
    }
    case SEPARATOR: {
      message = "separator";
      break;
    }
    case PREFIX_OP: {
      message = "prefix operator";
      break;
    }
    case BINARY_OP: {
      message = "binary operator";
      break;
    }
    case ASSIGN_OP: {
      message = "assignment";
      break;
    }
    case POSTFIX_OP: {
      message = "postfix operator";
      break;
    }
    default: {
      message = "should not reach here";
    }
  }

  std::printf(
    "%03d:%03d| %s (%s)\n",
    line,
    column,
    content.c_str(),
    message.c_str()
  );
}
