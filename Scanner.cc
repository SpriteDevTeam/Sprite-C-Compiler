#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include "List.h"
#include "Token.h"

void scanner(List<Token>* list, std::fstream &file) {
  std::filebuf* buf = file.rdbuf();
  int line = 1;
  int column = 1;

  while (buf->sgetc() != EOF) {
    std::string content(1, (char)buf->sbumpc());
    Token* token;
    column++;

    switch (content[0]) {
      // string literal
      case '"': {
        char ch;
        int err_line = line;
        int err_column = column;

        while ((ch = buf->sbumpc()) != EOF) {
          content += ch;
          column++;

          if (ch == '"') {
            token = new Token(STRING_LIT, content, line, column);
            break;
          }
          else if (ch == '\n') {
              line++;
              column = 1;
          }
        }

        if (buf->in_avail() == 0) {
          token = new Token(STRING_ERR, content, err_line, err_column);
        }
        break;
      }
      // char literal
      case '\'': {
        char ch;
        int err_line = line;
        int err_column = column;

        while ((ch = buf->sbumpc()) != EOF) {
          content += ch;
          column++;

          if (ch == '\'') {
            token = new Token(CHAR_LIT, content, line, column);
            break;
          }
          else if (ch == '\n') {
            break;
          }
        }

        if (ch == '\n') {
          content.pop_back();
          token = new Token(CHAR_ERR, content, err_line, err_column);
          line++;
          column = 1;
        }
        break;
      }
      // number
      case '0': case '1': case '2': case '3': case '4': case '5': case '6':
      case '7': case '8': case '9': {
        char ch;

        while ((ch = buf->sbumpc()) != EOF) {
          if (std::isdigit(ch)) {
            content += ch;
            column++;
          }
          else {
            buf->sungetc();
            token = new Token(NUMBER, content, line, column);
            break;
          }
        }
        break;
      }
      // whitespace
      case '\n': {
        line++;
        column = 1;
      }
      case ' ': case '\t': case '\0': {
        continue;
      }
      // separators
      case '(': case ')': case '[': case ']': case '{': case '}': case ',':
      case ';': {
        token = new Token(SEPARATOR, content, line, column);
        break;
      }
      // operators
      case '.': {
        token = new Token(BINARY_OP, ".", line, column);
        break;
      }
      case '~': {
        token = new Token(PREFIX_OP, "~", line, column);
        break;
      }
      case '+': {
        char ch = buf->sbumpc();

        if (ch == '+') {
          token = new Token(TBD, "++", line, ++column);
        }
        else if (ch == '=') {
          token = new Token(ASSIGN_OP, "+=", line, ++column);
        }
        else {
          buf->sungetc();
          token = new Token(TBD, "+", line, column);
        }
        break;
      }
      case '-': {
        char ch = buf->sbumpc();

        if (ch == '-') {
          token = new Token(TBD, "--", line, ++column);
        }
        else if (ch == '=') {
          token = new Token(ASSIGN_OP, "-=", line, ++column);
        }
        else if (ch == '>') {
          token = new Token(BINARY_OP, "->", line, ++column);
        }
        else {
          buf->sungetc();
          token = new Token(TBD, "-", line, column);
        }
        break;
      }
      case '*': {
        char ch = buf->sbumpc();

        if (ch == '=') {
          token = new Token(ASSIGN_OP, "*=", line, ++column);
        }
        else {
          buf->sungetc();
          token = new Token(TBD, "*", line, column);
        }
        break;
      }
      case '/': {
        char ch = buf->sbumpc();
        int err_line = line;
        int err_column = column;

        if (ch == '/') {
          while ((ch = buf->sbumpc()) != EOF) {
            if (ch == '\n') {
              line++;
              column = 1;
              break;
            }
          }
          continue;
        }
        else if (ch == '*') {
          while ((ch = buf->sbumpc()) != EOF) {
            column++;
            if (ch == '*') {
              ch = buf->sbumpc();
              column++;
              if (ch == '/') {
                break;
              }
            }
            if (ch == '\n') {
              line++;
              column = 1;
            }
          }
          if (buf->in_avail() == 0) {
            token = new Token(COMMENT_ERR, "/*", err_line, err_column+1);
          }
          continue;
        }
        else if (ch == '=') {
          token = new Token(ASSIGN_OP, "/=", line, ++column);
        }
        else {
          buf->sungetc();
          token = new Token(BINARY_OP, "/", line, column);
        }
        break;
      }
      case '=': {
        char ch = buf->sbumpc();

        if (ch == '=') {
          token = new Token(BINARY_OP, "==", line, ++column);
        }
        else {
          buf->sungetc();
          token = new Token(ASSIGN_OP, "=", line, column);
        }
        break;
      }
      case '<': {
        char ch = buf->sbumpc();

        if (ch == '<') {
          if (buf->sbumpc() == '=') {
            column += 2;
            token = new Token(ASSIGN_OP, "<<=", line, column);
          }
          else {
            buf->sungetc();
            token = new Token(BINARY_OP, "<<", line, ++column);
          }
        }
        else if (ch == '=') {
          token = new Token(ASSIGN_OP, "<=", line, ++column);
        }
        else {
          buf->sungetc();
          token = new Token(BINARY_OP, "<", line, column);
        }
        break;
      }
      case '>': {
        char ch = buf->sbumpc();

        if (ch == '>') {
          if (buf->sbumpc() == '=') {
            column += 2;
            token = new Token(ASSIGN_OP, ">>=", line, column);
          }
          else {
            buf->sungetc();
            token = new Token(BINARY_OP, ">>", line, ++column);
          }
        }
        else if (ch == '=') {
          token = new Token(ASSIGN_OP, ">=", line, ++column);
        }
        else {
          buf->sungetc();
          token = new Token(BINARY_OP, ">", line, column);
        }
        break;
      }
      case '!': {
        char ch = buf->sbumpc();

        if (ch == '=') {
          token = new Token(BINARY_OP, "!=", line, ++column);
        }
        else {
          buf->sungetc();
          token = new Token(PREFIX_OP, "!", line, column);
        }
        break;
      }
      case '&': {
        char ch = buf->sbumpc();

        if (ch == '&') {
          token = new Token(BINARY_OP, "&&", line, ++column);
        }
        else if (ch == '=') {
          token = new Token(ASSIGN_OP, "&=", line, ++column);
        }
        else {
          buf->sungetc();
          token = new Token(TBD, "&", line, column);
        }
        break;
      }
      case '|': {
        char ch = buf->sbumpc();

        if (ch == '|') {
          token = new Token(BINARY_OP, "||", line, ++column);
        }
        else if (ch == '=') {
          token = new Token(ASSIGN_OP, "|=", line, ++column);
        }
        else {
          buf->sungetc();
          token = new Token(BINARY_OP, "|", line, column);
        }
        break;
      }
      case '%': {
        char ch = buf->sbumpc();

        if (ch == '=') {
          token = new Token(ASSIGN_OP, "%=", line, ++column);
        }
        else {
          buf->sungetc();
          token = new Token(BINARY_OP, "%", line, column);
        }
        break;
      }
      case '^': {
        char ch = buf->sbumpc();

        if (ch == '=') {
          token = new Token(ASSIGN_OP, "^=", line, ++column);
        }
        else {
          buf->sungetc();
          token = new Token(BINARY_OP, "^", line, column);
        }
        break;
      }
      // id, keyword & unknown character
      default: {
        if (!std::isalpha(content[0]) && content[0] != '_') {
          token = new Token(UNKNOWN_ERR, content, line, column);
          break;
        }

        char ch;
        while ((ch = buf->sbumpc()) != EOF) {
          if (std::isalnum(ch) || ch == '_') {
            content += ch;
            column++;
          }
          else {
            buf->sungetc();

            if (content == "return")        goto keyword_token;
            else if (content == "printf")   goto keyword_token;
            else if (content == "scanf")    goto keyword_token;
            else if (content == "open")     goto keyword_token;
            else if (content == "close")    goto keyword_token;
            else if (content == "read")     goto keyword_token;
            else if (content == "write")    goto keyword_token;
            else if (content == "malloc")   goto keyword_token;
            else if (content == "free")     goto keyword_token;
            else if (content == "if")       goto keyword_token;
            else if (content == "else")     goto keyword_token;
            else if (content == "for")      goto keyword_token;
            else if (content == "break")    goto keyword_token;
            else if (content == "continue") goto keyword_token;
            else if (content == "enum")     goto keyword_token;
            else if (content == "struct")   goto keyword_token;
            else if (content == "int")      goto keyword_token;
            else if (content == "char")     goto keyword_token;
            else if (content == "void")     goto keyword_token;
            else                            goto id_token;
keyword_token:
            token = new Token(KEYWORD, content, line, column);
            break;
id_token:
            token = new Token(ID, content, line, column);
            break;
          }
        }
      }
    }

    if (list->tail == nullptr || list->tail->data != token) {
      ListNode<Token>* node = new ListNode<Token>(token);
      list->append(node);
    }
  }
}

void check_scan_error(List<Token>* list) {
  bool is_err_found = false;

  if (list->size > 0) {
    ListNode<Token>* curr = list->head;
    while (curr) {
      Token* data = curr->data;

      switch (data->type) {
        case COMMENT_ERR: {
          std::fprintf(
            stderr,
            "Lexical Error: comment not closed. (line %d, column %d)\n",
            data->line,
            data->column
          );
          is_err_found = true;
          break;
        }
        case CHAR_ERR: {
          std::fprintf(
            stderr,
            "Lexical Error: char literal not closed. (line %d, column %d)\n",
            data->line,
            data->column
          );
          is_err_found = true;
          break;
        }
        case STRING_ERR: {
          std::fprintf(
            stderr,
            "Lexical Error: string literal not closed. (line %d, column %d)\n",
            data->line,
            data->column
          );
          is_err_found = true;
          break;
        }
        case UNKNOWN_ERR: {
          std::fprintf(
            stderr,
            "Lexical Error: unknown character: %s (line %d, column %d)\n",
            data->content.c_str(),
            data->line,
            data->column
          );
          is_err_found = true;
          break;
        }
      }
      curr = curr->next;
    }
  }
  else {
    is_err_found = true;
  }

  if (is_err_found) {
    std::fprintf(stderr, "Parsing failed.\n");
    exit(1);
  }
}
