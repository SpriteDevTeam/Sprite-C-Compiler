#ifndef __SymbolTable_H__
#define __SymbolTable_H__

#include <map>
#include <string>
#include <vector>
#include "CST.h"

// * Each symbol has a record in the form of {key, info}.
//     key  := {category (int), name (string), type (string)}
//     info := {attributes (array of string), context (pointer)}
//
// function:
//    {{ST_FUNC, function name, return type}, {{[type1, name1, ...]}, context}}
// struct:
//    {{ST_STRUCT, struct name, struct}, {{type1, member1, ...}, nullptr}}
// enum:
//    {{ST_ENUM, enum, enum}, {{name1, [num1], ...}, nullptr}}}
// variable:
//    {{ST_VAR, variable name, int}, {{}, context}}
//    {{ST_VAR, variable name, char}, {{}, context}}
//    {{ST_VAR, variable name, struct name}, {{}, context}}
//    {{ST_VAR, variable name, int*}, {{}, context}}
//    {{ST_VAR, variable name, char*}, {{}, context}}
//    {{ST_VAR, variable name, struct name*}, {{}, context}}
//    {{ST_VAR, variable name, void}, {{}, context}}
//
// When it's necessary, always create a new symbol table before passing in to
// a function.

enum {
  ST_FUNC = 0,
  ST_KEYWORD_FUNC,
  ST_ENUM,
  ST_STRUCT,
  ST_VAR
};

enum {
  ST_ERR_FUNC_SIG = 0,
  ST_ERR_MUL_FUNC_DEF,
  ST_ERR_MUL_STRUCT_DEF,
  ST_ERR_NOT_DECL_BEFORE_USE,
  ST_ERR_NOT_LVALUE_WHILE_ASSIGN,
  ST_ERR_ARG_NUM_NOT_FIT,
  ST_ERR_ARG_TYPE_NOT_FIT,
  ST_ERR_TYPE_NOT_FIT,
  ST_ERR_NOT_POINTER_WHILE_ACCESS
};

using Key = std::tuple<int, std::string, std::string>;

struct SymbolTable;

// info.children should be used in function
struct Info {
  std::vector<std::string> attrs;
  CSTNode* context = nullptr;
  int addr = 0; // local var effective address
  std::vector<SymbolTable*> children;
};

// symbol_table.children should be used in statememt
struct SymbolTable {
  std::map<Key, Info*> data;
  SymbolTable* parent;
  std::vector<SymbolTable*> children;
  SymbolTable(SymbolTable* parent=nullptr): parent(parent) {}
  Info* lookup(int category, std::string name, std::string type);
  std::string ambiguous_lookup(int category, std::string name);
  void insert(Key key, Info* info);
};

SymbolTable* build_symbol_table(CSTNode* node);
void _build_symbol_table_global(SymbolTable* table, CSTNode* node);
void _build_symbol_table_local(SymbolTable* table, CSTNode* node);
void display_symbol_table(SymbolTable* table);
void _display_symbol_table(SymbolTable* table, std::string prefix, bool is_global);
void destruct_symbol_table(SymbolTable* table);

bool is_const_expr(SymbolTable* table, CSTNode* node);
bool evaluate(CSTNode* node);
std::string resolve_expr(SymbolTable* table, CSTNode* node);
std::string resolve_lvalue(SymbolTable* table, CSTNode* node);

void resolve_error(int err_code); // will exit the program

#endif
