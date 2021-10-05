#include <map>
#include <string>
#include <vector>
#include "SymbolTable.h"

Info* SymbolTable::lookup(int category, std::string name, std::string type) {
  for (SymbolTable* curr = this; curr != nullptr; curr = curr->parent) {
    auto result = curr->data.find({category, name, type});
    if (result != curr->data.end()) {
      return result->second;
    }
  }
  return nullptr;
}

// ambiguous_lookup should be used while category and name are known
std::string SymbolTable::ambiguous_lookup(int category, std::string name) {
  for (SymbolTable* curr = this; curr != nullptr; curr = curr->parent) {
    for (auto i = curr->data.begin(); i != curr->data.end(); i++) {
      Key key = i->first;
      int test_category;
      std::string test_name;
      std::string test_type;
      std::tie(test_category, test_name, test_type) = key;

      if (test_category == category && test_name == name) {
        return test_type;
      }
    }
  }
  return "";
}

void SymbolTable::insert(Key key, Info* info) {
  data.insert({key, info});
}

SymbolTable* build_symbol_table(CSTNode* node) {
  SymbolTable* table = new SymbolTable();
  _build_symbol_table_global(table, node);
  return table;
}

void _build_symbol_table_global(SymbolTable* table, CSTNode* node) {
  table->insert({ST_KEYWORD_FUNC, "open", "int"}, new Info{{"char *", "filename"}, nullptr});
  table->insert({ST_KEYWORD_FUNC, "close", "void"}, new Info{{"int", "fd"}, nullptr});
  table->insert({ST_KEYWORD_FUNC, "read", "void"}, new Info{{"int", "fd", "char *", "buf", "int", "nbytes"}, nullptr});
  table->insert({ST_KEYWORD_FUNC, "write", "void"}, new Info{{"int", "fd", "char *", "buf", "int", "nbytes"}, nullptr});
  table->insert({ST_KEYWORD_FUNC, "malloc", "void *"}, new Info{{"int", "nbytes"}, nullptr});
  table->insert({ST_KEYWORD_FUNC, "free", "void"}, new Info{{"void *", "ptr"}, nullptr});
  for (auto i : node->children) {
    // decl
    if (i->type == DECL) {
      CSTNode* decl_node = i;
      // func decl
      if (decl_node->children[2]->type == _LEFT_PAREN) {
        CSTNode* id_node   = decl_node->children[1];
        CSTNode* type_node = decl_node->children[0];

        // prepare key
        int category = ST_FUNC;
        std::string name = id_node->content;
        std::string type = type_node->content;

        // prepare info
        Info* info = new Info();
        if (decl_node->children[3]->type == ARG_LIST) {
          CSTNode* arg_list_node = decl_node->children[3];
          for (auto i : arg_list_node->children) {
            info->attrs.push_back(i->content);
          }
        }

        table->insert({category, name, type}, info);
      }
      // var decl
      else if (decl_node->children[0]->type == TYPE) {
        CSTNode* id_node   = decl_node->children[1];
        CSTNode* type_node = decl_node->children[0];

        // prepare key
        int category = ST_VAR;
        std::string name = id_node->content;
        std::string type = type_node->content;

        Info* info = new Info();
        if (decl_node->children.size() > 2) {
          info->context = decl_node->children[3];
          std::string expr_type = resolve_expr(table, info->context);
          if ((type != expr_type) &&
              !(expr_type == "void *" && type.back() == '*') &&
              !(type == "void *" && expr_type.back() == '*')) {
            resolve_error(ST_ERR_TYPE_NOT_FIT);
          }
          //_build_symbol_table_local(table, info->context);
        }
        table->insert({category, name, type}, info);
      }
      // struct decl
      else if (decl_node->children[0]->type == _STRUCT) {
        CSTNode* id_node   = decl_node->children[1];
        CSTNode* type_node = decl_node->children[0];

        // prepare key
        int category = ST_STRUCT;
        std::string name = "struct " + id_node->content;
        std::string type = "struct";

        Info* info = new Info();
        table->insert({category, name, type}, info);
      }
      // enum decl
      else {
        // prepare key
        int category = ST_ENUM;
        std::string name = "enum";
        std::string type = "enum";

        Info* info = new Info();
        if (decl_node->children[2]->type == ENUM_LIST) {
          for (auto i : decl_node->children[2]->children) {
            info->attrs.push_back(i->content);
          }
        }
        table->insert({category, name, type}, info);
      }
    }
    // def
    else if (i->type == DEF) {
      CSTNode* def_node = i;
      // func def
      if (def_node->children[2]->type == _LEFT_PAREN) {
        CSTNode* id_node   = def_node->children[1];
        CSTNode* type_node = def_node->children[0];

        // prepare key
        int category = ST_FUNC;
        std::string name = id_node->content;
        std::string type = type_node->content;

        Info* info;
        // check if the function is already declared
        if ((info = table->lookup(category, name, type)) != nullptr) {
          if (info->context != nullptr) {
            resolve_error(ST_ERR_MUL_FUNC_DEF);
          }
          if (def_node->children[3]->type == ARG_LIST) {
            CSTNode* arg_list_node = def_node->children[3];

            if (info->attrs.size() != arg_list_node->children.size()) {
              resolve_error(ST_ERR_FUNC_SIG);
            }
            for (int i = 0; i < int(info->attrs.size()); i++) {
              if (info->attrs[i] != arg_list_node->children[i]->content) {
                resolve_error(ST_ERR_FUNC_SIG);
              }
            }

            info->context = def_node->children[6];
            SymbolTable* new_table = new SymbolTable(table);
            info->children.push_back(new_table);

            // put parameter into local symbol table
            int category = ST_VAR;
            std::string name;
            std::string type;
            int effective_addr = -8; // first 8 bytes are ra and s0(fp)
            for (int i = 0; i < int(info->attrs.size()); i+=2) {
              name = info->attrs[i+1];
              type = info->attrs[i];
              new_table->insert({category, name, type}, new Info{{}, nullptr, (effective_addr-=4)});
            }

            _build_symbol_table_local(new_table, info->context);
          }
          else {
            if (info->attrs.size() != 0) {
              resolve_error(ST_ERR_FUNC_SIG);
            }
            info->context = def_node->children[5];
            SymbolTable* new_table = new SymbolTable(table);
            info->children.push_back(new_table);
            _build_symbol_table_local(new_table, info->context);
          }
        }
        else {
          // prepare info
          info = new Info();
          if (def_node->children[3]->type == ARG_LIST) {
            CSTNode* arg_list_node = def_node->children[3];

            for (auto i : arg_list_node->children) {
              info->attrs.push_back(i->content);
            }
            info->context = def_node->children[6];
            SymbolTable* new_table = new SymbolTable(table);
            info->children.push_back(new_table);

            // put parameter into local symbol table
            int category = ST_VAR;
            std::string name;
            std::string type;
            int effective_addr = -8; // first 8 bytes are ra and s0(fp)
            for (int i = 0; i < int(info->attrs.size()); i+=2) {
              name = info->attrs[i+1];
              type = info->attrs[i];
              new_table->insert({category, name, type}, new Info{{}, nullptr, (effective_addr-=4)});
            }

            _build_symbol_table_local(new_table, info->context);
          }
          else {
            info->context = def_node->children[5];
            SymbolTable* new_table = new SymbolTable(table);
            info->children.push_back(new_table);
            _build_symbol_table_local(new_table, info->context);
          }

          table->insert({category, name, type}, info);
        }
      }
      // struct def
      else if (def_node->children[0]->type == _STRUCT) {
        CSTNode* id_node   = def_node->children[1];
        CSTNode* type_node = def_node->children[0];

        // prepare key
        int category = ST_STRUCT;
        std::string name = "struct " + id_node->content;
        std::string type = "struct";

        Info* info;
        // check if the struct is already declared
        if ((info = table->lookup(category, name, type)) != nullptr) {
          if (info->attrs.size()) {
            resolve_error(ST_ERR_MUL_STRUCT_DEF);
          }

          if (def_node->children[3]->type == MEM_DECL_LIST) {
            for (auto i : def_node->children[3]->children) {
              info->attrs.push_back(i->content);
            }
          }
          else {
            ;
          }
        }
        else {
          info = new Info();
          if (def_node->children[3]->type == MEM_DECL_LIST) {
            for (auto i : def_node->children[3]->children) {
              info->attrs.push_back(i->content);
            }
          }
          table->insert({category, name, type}, info);
        }
      }
    }
  }
}

void _build_symbol_table_local(SymbolTable* table, CSTNode* node) {
  if (node->type == STMT_LIST) {
    for (auto i : node->children) {
      _build_symbol_table_local(table, i);
    }
  }
  else if (node->type == STMT) {
    // var_decl
    if (node->children[0]->type == TYPE) {
      CSTNode* type_node = node->children[0];
      CSTNode* id_node = node->children[1];
      // prepare key
      int category = ST_VAR;
      std::string name = id_node->content;
      std::string type = type_node->content;

      Info* info = new Info();
      if (node->children.size() > 2) {
        info->context = node->children[3];
        std::string expr_type = resolve_expr(table, info->context);
        if ((type != expr_type) &&
            !(expr_type == "void *" && type.back() == '*') &&
            !(type == "void *" && expr_type.back() == '*')) {
          resolve_error(ST_ERR_TYPE_NOT_FIT);
        }
      }
      info->addr = -8 - 4*(table->data.size()+1); // first 8 bytes are ra and s0(fp)
      table->insert({category, name, type}, info);
    }
    else if (node->children[0]->type == _IF) {
      resolve_expr(table, node->children[2]);
      _build_symbol_table_local(table, node->children[4]);

      if (node->children.size() > 5) {
        _build_symbol_table_local(table, node->children[6]);
      }
    }
    else if (node->children[0]->type == _FOR) {
      resolve_expr(table, node->children[2]);
      resolve_expr(table, node->children[3]);
      resolve_expr(table, node->children[4]);
      _build_symbol_table_local(table, node->children[6]);
    }
    else if (node->children[0]->type == _RETURN) {
      if (node->children.size() > 1)
        resolve_expr(table, node->children[1]);
    }
    else if (node->children[0]->type == EXPR) {
      resolve_expr(table, node->children[0]);
    }
    else if (node->children[0]->type == _LEFT_BRACE) {
      SymbolTable* new_table = new SymbolTable(table);
      table->children.push_back(new_table);
      _build_symbol_table_local(new_table, node->children[1]);
    }
  }
}

void display_symbol_table(SymbolTable* table) {

  _display_symbol_table(table, "", true);
}

void _display_symbol_table(SymbolTable* table, std::string prefix, bool is_global) {
  int c = 1;
  std::cout << prefix + "+----------------------------" << std::endl;
  std::cout << prefix + "|Symbol Table: " << std::endl;
  std::cout << prefix + "+----------------------------" << std::endl;
  prefix += "|";
  for (auto i = table->data.begin(); i != table->data.end(); i++) {
    Key key = i->first;
    Info* info = i->second;
    int category;
    std::string name;
    std::string type;
    std::tie(category, name, type) = key;

    switch (category) {
      case ST_FUNC: {
        std::cout << prefix + "function: " << type << " " << name << std::endl;
        break;
      }
      case ST_KEYWORD_FUNC: {
        std::cout << prefix + "keyword_function: " << type << " " << name << std::endl;
        break;
      }
      case ST_ENUM: {
        std::cout << prefix + "enum: " << std::endl;
        break;
      }
      case ST_STRUCT: {
        std::cout << prefix + "struct: " << name << std::endl;
        break;
      }
      case ST_VAR: {
        std::cout << prefix + "variable: " << type << " " << name << std::endl;
        break;
      }
    }
    std::cout << prefix + "    attr: \n";
    if (info->attrs.size()) {
      std::cout << prefix + "        ";
      for (auto x : info->attrs) {
        std::cout << x << " ";
      }
      std::cout << std::endl;
    }
    else
      std::cout << prefix + "        None" << std::endl;
    if (category == ST_FUNC || category == ST_VAR) {
      std::cout << prefix + "    context: \n";
      if (info->context)
        _display_CST(info->context, prefix + "        ", true);
      else
        std::cout << prefix + "        None" << std::endl;

      if (category == ST_VAR && !is_global) {
        std::cout << prefix + "    effective address: \n";
        std::cout << prefix + "        " << info->addr << std::endl;
      }
    }

    if (is_global) {
      for (auto i : info->children) {
        _display_symbol_table(i, prefix + "        ", false);
      }
    }
  }

  if (!is_global) {
    for (auto i : table->children) {
      _display_symbol_table(i, prefix + "        ", false);
    }
  }

  prefix.pop_back();
  std::cout << prefix + "+----------------------------" << std::endl;
  prefix += "|";
}

void destruct_symbol_table() {

}

std::string resolve_expr(SymbolTable* table, CSTNode* node) {
  // +- <terminal>
  if (node->type == EXPR && node->children.size() == 1) {
    return resolve_expr(table, node->children[0]);
  }
  else if (node->type == NUMBER) {
    std::cout << node->content << std::endl;
    return "int";
  }
  else if (node->type == STRING_LIT) {
    std::cout << node->content << std::endl;
    return "char *";
  }
  else if (node->type == CHAR_LIT) {
    std::cout << node->content << std::endl;
    return "char";
  }
  // +- <lvalue>
  else if (node->type == LVALUE) {
    return resolve_lvalue(table, node);
  }
  // +- <expr>
  //    +- <keyword_func>
  //    +- <_left_paren>
  //    +- <para_list> (optional)
  //       +- <expr>
  //       +- <expr>
  //    +- <_right_paren>
  else if (node->children[0]->type == KEYWORD_FUNC) {
    std::string ret_type = table->ambiguous_lookup(ST_KEYWORD_FUNC, node->children[0]->content);
    Info* func_info = table->lookup(ST_KEYWORD_FUNC, node->children[0]->content, ret_type);
    if (func_info->attrs.size() != node->children[2]->children.size()*2) {
      resolve_error(ST_ERR_ARG_NUM_NOT_FIT);
    }
    for (int i = 0; i < int(func_info->attrs.size()); i += 2) {
      std::cout << func_info->attrs[i] << std::endl;
      std::cout << resolve_expr(table, node->children[2]->children[i/2]) << std::endl;
      std::string arg_type = func_info->attrs[i];
      std::string para_type = resolve_expr(table, node->children[2]->children[i/2]);
      if ((arg_type != para_type) &&
          !(para_type == "void *" && arg_type.back() == '*') &&
          !(arg_type == "void *" && para_type.back() == '*')) {
        resolve_error(ST_ERR_ARG_TYPE_NOT_FIT);
      }
    }
    return ret_type;
  }
  // + <expr>
  //   +- <lvalue>
  //   |  +- <id>
  //   +- <_left_paren>
  //   +- <para_list> (optional)
  //   +- <_right_paren>
  else if (node->children.size() > 1 &&
           node->children[0]->type == LVALUE &&
           node->children[1]->type == _LEFT_PAREN) {
    std::string ret_type = table->ambiguous_lookup(ST_FUNC, node->children[0]->children[0]->content);
    if (ret_type == "") {
      resolve_error(ST_ERR_NOT_DECL_BEFORE_USE);
    }
    Info* func_info = table->lookup(ST_FUNC, node->children[0]->children[0]->content, ret_type);
    if (func_info->attrs.size() != node->children[2]->children.size()*2) {
      resolve_error(ST_ERR_ARG_NUM_NOT_FIT);
    }
    for (int i = 0; i < int(func_info->attrs.size()); i += 2) {
      std::cout << func_info->attrs[i] << std::endl;
      std::cout << resolve_expr(table, node->children[2]->children[i/2]) << std::endl;
      std::string arg_type = func_info->attrs[i];
      std::string para_type = resolve_expr(table, node->children[2]->children[i/2]);
      if ((arg_type != para_type) &&
          !(para_type == "void *" && arg_type.back() == '*') &&
          !(arg_type == "void *" && para_type.back() == '*')) {
        resolve_error(ST_ERR_ARG_TYPE_NOT_FIT);
      }
    }
    return ret_type;
  }
  else if (node->children.size() == 2) {
    // arithmetic
    // +- <expr>
    //    +- <unary_op>
    //    +- <expr>
    if (node->children[0]->type == UNARY_OP) {
      std::string type = resolve_expr(table, node->children[1]);
      if (type != "int"){
        resolve_error(ST_ERR_TYPE_NOT_FIT);
      }
      std::cout << node->children[0]->content << std::endl;
      return type;
    }
    // address-of and dereference
    // should not reach here
    else if (node->children[0]->type == _ASTERISK ||
             node->children[0]->type == _AMPERSAND) {
      std::cerr << "resolve_expr: reach address-of and dereference" << std::endl;
    }
  }
  else if (node->children.size() == 3) {
    // arithmetic
    // +- <expr>
    //    +- <expr>
    //    +- <eq_op, ..., mul_op>
    //    +- <expr>
    if (node->children[1]->type == EQ_OP ||
        node->children[1]->type == COND_OP ||
        node->children[1]->type == SHIFT_OP ||
        node->children[1]->type == ADD_OP ||
        node->children[1]->type == MUL_OP) {
      std::string first_type = resolve_expr(table, node->children[0]);
      std::string second_type = resolve_expr(table, node->children[2]);
      if (first_type != second_type) {
        resolve_error(ST_ERR_TYPE_NOT_FIT);
      }
      std::cout << node->children[1]->content << std::endl;
      return first_type;
    }
    // assignment
    // +- <expr>
    //    +- <lvalue>
    //    +- <_equals> (see CST.cc:fix_CST() case 5)
    //    +- <expr>
    else if (node->children[1]->type == _EQUALS) {
      std::string lvalue_type = resolve_lvalue(table, node->children[0]);
      std::string expr_type = resolve_expr(table, node->children[2]);
      std::cout << expr_type << std::endl;
      if ((lvalue_type != expr_type) &&
          !(expr_type == "void *" && lvalue_type.back() == '*') &&
          !(lvalue_type == "void *" && expr_type.back() == '*')) {
        resolve_error(ST_ERR_TYPE_NOT_FIT);
      }
      std::cout << node->children[1]->content << std::endl;
      std::cout << lvalue_type << std::endl;
      return lvalue_type;
    }
    // member access
    // should not reach here
    else if (node->children[1]->type == _ARROW ||
             node->children[1]->type == _DOT) {
      std::cerr << "resolve_expr: reach member access" << std::endl;
    }
  }

  else if (node->children.size() == 4) {
    // arithmetic
    //+- <expr>
    //   +- <lvalue>
    //      +- <id> ==> arr
    //   +- <_left_bracket> ==> [
    //   +- <expr>
    //   +- <_right_bracket> ==> ]
    std::string lvalue_type = resolve_lvalue(table, node->children[0]);
    if (lvalue_type == "") {
      resolve_error(ST_ERR_NOT_DECL_BEFORE_USE);
    }
    else if (lvalue_type.back() != '*') {
      resolve_error(ST_ERR_NOT_POINTER_WHILE_ACCESS);
    }
    std::string index_type = resolve_expr(table, node->children[2]);
    if (index_type != "int") {
      resolve_error(ST_ERR_TYPE_NOT_FIT);
    }
    return lvalue_type.substr(0, lvalue_type.size()-2);
  }
  else {
    // should not reach here
    std::cerr << "resolve_expr: unknown node: " << node->type << " " << node->content << std::endl;
  }
  return "";
}

std::string resolve_lvalue(SymbolTable* table, CSTNode* node) {
  // +- <lvalue>
  //    +- <id>
  if (node->children.size() == 1) {
    std::string var_type = table->ambiguous_lookup(ST_VAR, node->children[0]->content);
    if (var_type == "") {
      resolve_error(ST_ERR_NOT_DECL_BEFORE_USE);
    }
    return var_type;
  }
  // +- <lvalue>
  //    +- <_asterisk or _ampersand>
  //    +- <lvalue>
  else if (node->children.size() == 2) {
    ;
  }
  // +- <lvalue>
  //    +- <lvalue>
  //    +- <_dot or _arrow>
  //    +- <lvalue>
  else if (node->children.size() == 3) {
    ;
  }
  // +- <lvalue>
  //    +- <lvalue>
  //    +- <_left_bracket>
  //    +- <expr>
  //    +- <_right_bracket>
  else if (node->children.size() == 4) {
    std::string lvalue_type = resolve_lvalue(table, node->children[0]);
    if (lvalue_type == "") {
      resolve_error(ST_ERR_NOT_DECL_BEFORE_USE);
    }
    else if (lvalue_type.back() != '*') {
      resolve_error(ST_ERR_NOT_POINTER_WHILE_ACCESS);
    }
    std::string index_type = resolve_expr(table, node->children[2]);
    if (index_type != "int") {
      resolve_error(ST_ERR_TYPE_NOT_FIT);
    }
    return lvalue_type.substr(0, lvalue_type.size()-2);
  }
  else {
    ;
  }
  return "";
}

void resolve_error(int err_code) {
  std::cerr << "Resolve error: ";
  switch (err_code) {
    case ST_ERR_FUNC_SIG: {
      std::cerr << "ST_ERR_FUNC_SIG" << std::endl;
      break;
    }
    case ST_ERR_MUL_FUNC_DEF: {
      std::cerr << "ST_ERR_MUL_FUNC_DEF" << std::endl;
      break;
    }
    case ST_ERR_MUL_STRUCT_DEF: {
      std::cerr << "ST_ERR_MUL_STRUCT_DEF" << std::endl;
      break;
    }
    case ST_ERR_NOT_DECL_BEFORE_USE: {
      std::cerr << "ST_ERR_NOT_DECL_BEFORE_USE" << std::endl;
      break;
    }
    case ST_ERR_NOT_LVALUE_WHILE_ASSIGN: {
      std::cerr << "ST_ERR_NOT_LVALUE_WHILE_ASSIGN" << std::endl;
      break;
    }
    case ST_ERR_ARG_NUM_NOT_FIT: {
      std::cerr << "ST_ERR_ARG_NUM_NOT_FIT" << std::endl;
      break;
    }
    case ST_ERR_ARG_TYPE_NOT_FIT: {
      std::cerr << "ST_ERR_ARG_TYPE_NOT_FIT" << std::endl;
      break;
    }
    case ST_ERR_TYPE_NOT_FIT: {
      std::cerr << "ST_ERR_TYPE_NOT_FIT" << std::endl;
      break;
    }
    case ST_ERR_NOT_POINTER_WHILE_ACCESS: {
      std::cerr << "ST_ERR_NOT_POINTER_WHILE_ACCESS" << std::endl;
      break;
    }
    default: {
      std::cerr << "ST_ERR_UNKNOWN" << std::endl;
      break;
    }
  }
  exit(1);
}
