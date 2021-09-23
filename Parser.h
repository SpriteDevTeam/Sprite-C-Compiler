#ifndef __PARSER_H__
#define __PARSER_H__

#include <iostream>
#include <string>
#include "CST.h"
#include "List.h"
#include "Token.h"

void parser(List<Token>* list);

CSTNode* build_CST(List<Token>* list);
void destruct_CST(CSTNode* node);

void display_CST(CSTNode* node);

void _display_CST(CSTNode* node, std::string prefix, bool is_the_last);

#endif
