#ifndef __TOKEN_H__
#define __TOKEN_H__

#include <iostream>
#include <string>

// For token definitions, see: https://git.io/Juwdb
//
// Appendix A.
//     Table of naming of symbolic tokens
//
//     token               name of token                  note
//     -------------------------------------------------------
//     (                      left paren          expr or func
//     )                     right paren          expr or func
//     [                    left bracket                 array
//     ]                   right bracket                 array
//     {                      left brace                 scope
//     }                     right brace                 scope
//     ,                           comma                  func
//     ;                       semicolon             separator
//     .                             dot                struct
//     ~                           tilde                prefix
//     +                            plus      prefix or binary
//     ++                    double plus     prefix or postfix
//     +=                    plus equals            assignment
//     -                           minus      prefix or binary
//     --                   double minus     prefix or postfix
//     -=                   minus equals            assignment
//     ->                          arrow               pointer
//     *                        asterisk     pointer or binary
//     *=                asterisk equals            assignment
//     */                 asterisk slash               comment
//     /                           slash                binary
//     //                   double slash               comment
//     /*                 slash asterisk               comment
//     /=                   slash equals            assignment
//     =                          equals            assignment
//     ==                  double equals                binary
//     <                       less than                binary
//     <<               double less than                binary
//     <=               less than equals            assignment
//     <<=       double less than equals            assignment
//     >                    greater than                binary
//     >>            double greater than                binary
//     >=            greater than equals            assignment
//     >>=    double greater than equals            assignment
//     !                     exclamation                prefix
//     !=             exclamation equals                binary
//     &                       ampersand     pointer or binary
//     &&               double ampersand                binary
//     &=               ampersand equals            assignment
//     |                    vertical bar                binary
//     ||            double vertical bar                binary
//     |=            vertical bar equals            assignment
//     %                         percent                binary
//     %=                 precent equals            assignment
//     ^                           caret                binary
//     ^=                   caret equals            assignment

#define COMMENT_ERR -4
#define CHAR_ERR    -3
#define STRING_ERR  -2
#define UNKNOWN_ERR -1
#define TBD          0
#define KEYWORD      1
#define ID           2
#define NUMBER       3
#define STRING_LIT   4
#define CHAR_LIT     5
#define SEPARATOR    6
#define PREFIX_OP    7
#define BINARY_OP    8
#define ASSIGN_OP    9
#define POSTFIX_OP  10

struct Token {
  int type;
  std::string content;
  int line;
  int column;
  Token(int _type, std::string _content, int _line, int _column);
  void display();
};

#endif
