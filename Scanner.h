#ifndef __SCANNER_H__
#define __SCANNER_H__

#include <fstream>
#include "List.h"
#include "Token.h"

void scanner(List<Token>* list, std::fstream &file); // won't cause exit
void check_scan_error(List<Token>* list); // exit here if necessary

#endif
