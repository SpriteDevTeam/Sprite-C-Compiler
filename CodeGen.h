#ifndef __CODEGEN_H__
#define __CODEGEN_H__

#include <string>
#include "CST.h"
#include "SymbolTable.h"

void gen_code(SymbolTable* table);

void gen_global_var(const std::string& name, const std::string& type, Info* info);
void gen_func_def(const std::string& name, Info* info, SymbolTable* table, CSTNode* context);
void gen_stmt(const std::string& func_name, SymbolTable* table, CSTNode* stmt);
void gen_expr(SymbolTable* table, CSTNode* expr);
void gen_func_call(SymbolTable* table, std::string callee_name, CSTNode* para_list);
void _gen_instruction(const std::string& ins, const std::string& target, const std::string& arg1 = "", const std::string& arg2 = "");

#endif
