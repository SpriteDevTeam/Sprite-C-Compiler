#include <iostream>
#include "CodeGen.h"

static int _get_effective_addr(SymbolTable* table, std::string name);
static void _gen_binary_op(std::string op);

void gen_code(SymbolTable* table) {
  int category;
  std::string name;
  std::string type;
  Info info;
  for (auto& symbol : table->data) {
    std::tie(category, name, type) = symbol.first;
    Info* info = symbol.second;
    if (category == ST_FUNC) {
      gen_func_def(name, info, info->children[0], info->context);

    }
    else if (category == ST_VAR) {
      gen_global_var(name, type, info);
    }
  }
}

void gen_global_var(const std::string& name, const std::string& type, Info* info) {
  std::cout << name << ":" << std::endl;
  if (type == "char") {
    printf("        %-8s%d\n", ".byte", info->context->children[0]->content[1]);
  }
  else if (type == "int") {
    printf("        %-8s%s\n", ".word", info->context->children[0]->content.c_str());
  }
}

void gen_func_def(const std::string& name, Info* info, SymbolTable* table, CSTNode* context) {
  //display_CST(context);
  int frame_size = 8 + 4*table->data.size();
  //prologue
  std::cout << name << ":" << std::endl;
  _gen_instruction("addi", "sp", "sp", std::to_string(-frame_size));
  _gen_instruction("sw",   "ra", std::to_string(frame_size-4)+"(sp)");
  _gen_instruction("sw",   "s0", std::to_string(frame_size-8)+"(sp)");
  _gen_instruction("addi", "s0", "sp", std::to_string(frame_size));
  // set parameter
  for (int i = 0; i < (int)info->attrs.size(); i+=2) {
    int effective_addr = _get_effective_addr(table, info->attrs[i+1]);
    // local
    if (effective_addr != 0){
      _gen_instruction("sw",   "a"+std::to_string(i/2), std::to_string(effective_addr)+"(s0)");
    }
    // global
    else {
        std::cout << "global var unfinished" << std::endl;
    }
  }

  for (auto& stmt : context->children) {
    //if (stmt->type == STMT) {
    //  std::cout << "a statement:" << std::endl;
    //}
    gen_stmt(name, table, stmt);
  }
  //epilogue
  std::cout << "."+name+"_EPILOGUE" << ":" << std::endl;
  _gen_instruction("lw",   "ra", std::to_string(frame_size-4)+"(sp)");
  _gen_instruction("lw",   "s0", std::to_string(frame_size-8)+"(sp)");
  _gen_instruction("addi", "sp", "sp", std::to_string(frame_size));
  _gen_instruction("jr", "ra");
}

void gen_stmt(const std::string& func_name, SymbolTable* table, CSTNode* stmt) {
  if (stmt->children.size() > 0) {
    // decl and init
    if (stmt->children[0]->type == TYPE && stmt->children.size() > 2) {
      std::string lvalue_name = stmt->children[1]->content;
      int effective_addr = _get_effective_addr(table, lvalue_name);
      //std::cout << "var: " << lvalue_name << std::endl;
      //std::cout << "effect_addr: " << effective_addr << std::endl;
      gen_expr(table, stmt->children[3]);
      _gen_instruction("lw", "t1", "0(sp)");
      _gen_instruction("addi", "sp", "sp", "4"); // pop the last result
      // local int
      if (effective_addr != 0){
        _gen_instruction("sw", "t1", std::to_string(effective_addr)+"(s0)");
      }
      // global
      else {
          std::cout << "global var unfinished" << std::endl;
      }
    }
    // expr
    else if (stmt->children[0]->type == EXPR) {
      gen_expr(table, stmt->children[0]);
      _gen_instruction("addi", "sp", "sp", "4"); // pop the last result
    }
    // return
    else if (stmt->children[0]->type == _RETURN) {
      if (stmt->children.size() > 1) {
        gen_expr(table, stmt->children[1]);
        _gen_instruction("lw", "a0", "0(sp)");
        _gen_instruction("addi", "sp", "sp", "4"); // pop the last result
      }
      _gen_instruction("j", "."+func_name+"_EPILOGUE");
    }
  }
}

void gen_expr(SymbolTable* table, CSTNode* expr) {
  if (expr->children.size() == 1) {
    // +- <expr>
    //    +- <number>
    if (expr->children[0]->type == NUMBER) {
      // push
      _gen_instruction("addi", "sp", "sp", "-4");
      //zero
      if (expr->children[0]->content == "0") {
        _gen_instruction("sw", "zero", "0(sp)");

      }
      // non-zero
      else {
        _gen_instruction("li", "t1", expr->children[0]->content);
        _gen_instruction("sw", "t1", "0(sp)");
      }
    }
    // +- <lvalue>
    //    +- <id>
    else if (expr->children[0]->type == ID) {
      // push
      _gen_instruction("addi", "sp", "sp", "-4");

      std::string var_name = expr->children[0]->content;
      int effective_addr = _get_effective_addr(table, var_name);
      // local
      if (effective_addr != 0){
        _gen_instruction("lw", "t1", std::to_string(effective_addr)+"(s0)");
        _gen_instruction("sw", "t1", "0(sp)");
      }
      // global
      else {
          std::cout << "global var unfinished" << std::endl;
      }
    }
    // +- <expr>
    //    +- <lvalue>
    //       +- <id>
    else if (expr->children[0]->type == LVALUE) {
      gen_expr(table, expr->children[0]); // recursive to the above one
    }
  }
  // binary op
  else if (expr->children.size() == 3) {

    if (expr->children[1]->content != "=") {
      gen_expr(table, expr->children[0]); // lhs
      gen_expr(table, expr->children[2]); // rhs
      _gen_binary_op(expr->children[1]->content);
    }
    // assign (only local int now)
    else {
      gen_expr(table, expr->children[2]); // assignment need rhs value and lhs address
      int effective_addr = _get_effective_addr(table, expr->children[0]->children[0]->content);
      // pop one push one (only rhs)
      _gen_instruction("lw", "t2", "0(sp)"); // pop
      _gen_instruction("sw", "t2", std::to_string(effective_addr)+"(s0)"); // set local variable
      //_gen_instruction("sw", "t2", "0(sp)"); // push
    }
  }
  // call function
  else if (expr->children.size() == 4) {
    _gen_instruction("addi", "sp", "sp", "-4"); //push (for return value)
    gen_func_call(table, expr->children[0]->children[0]->content, expr->children[2]);
  }

}

void gen_func_call(SymbolTable* table, std::string callee_name, CSTNode* para_list) {
  std::string ret_type = table->ambiguous_lookup(ST_FUNC, callee_name);
  Info* func_info = table->lookup(ST_FUNC, callee_name, ret_type);
  // parameter list, at most 8 parameter now
  for (int i = func_info->attrs.size()-1; i > 0; i-=2) {
    gen_expr(table, para_list->children[i/2]);
    _gen_instruction("lw", "a"+std::to_string(i/2), "0(sp)"); // set parameter
    _gen_instruction("addi", "sp", "sp", "4"); // pop
  }
  _gen_instruction("call", callee_name);
  _gen_instruction("sw", "a0", "0(sp)");
}

// not including assign
static void _gen_binary_op(std::string op) {
  // pop two, push one
  _gen_instruction("lw", "t1", "4(sp)");
  _gen_instruction("lw", "t2", "0(sp)");
  _gen_instruction("addi", "sp", "sp", "4"); // + 8 - 4

  if (op == "+")
    _gen_instruction("add", "t1", "t1", "t2");
  else if (op == "-")
    _gen_instruction("sub", "t1", "t1", "t2");
  else if (op == "<")
    _gen_instruction("slt", "t1", "t1", "t2");
  else if (op == ">")
    _gen_instruction("sgt", "t1", "t1", "t2");
  else if (op == "<=") {
    _gen_instruction("sgt", "t1", "t1", "t2");
    _gen_instruction("xori", "t1", "t1", "1");
  }
  else if (op == ">=") {
    _gen_instruction("slt", "t1", "t1", "t2");
    _gen_instruction("xori", "t1", "t1", "1");
  }
  else if (op == "==") {
    _gen_instruction("sub", "t1", "t1", "t2");
    _gen_instruction("sltiu", "t1", "t1", "1");
  }
  else if (op == "!=") {
    _gen_instruction("sub", "t1", "t1", "t2");
    _gen_instruction("sltu", "t1", "zero", "t1");
  }
  else if (op == "*")
    _gen_instruction("unfinished: mul ", "t1", "t1", "t2");
  else if (op == "/")
    _gen_instruction("unfinished: div ", "t1", "t1", "t2");
  else if (op == "%")
    _gen_instruction("unfinished: rem ", "t1", "t1", "t2");

  _gen_instruction("sw", "t1", "0(sp)");
}

void _gen_instruction(const std::string& ins, const std::string& target, const std::string& arg1, const std::string& arg2) {
  if (arg1 == "") {
    printf("        %-8s%s\n", ins.c_str(), target.c_str());
  }
  else if (arg2 == "") {
    printf("        %-8s%s,%s\n", ins.c_str(), target.c_str(), arg1.c_str());
  }
  else {
    printf("        %-8s%s,%s,%s\n", ins.c_str(), target.c_str(), arg1.c_str(), arg2.c_str());
  }
}

static int _get_effective_addr(SymbolTable* table, std::string name) {
  std::string var_type = table->ambiguous_lookup(ST_VAR, name);
  Info* var_info = table->lookup(ST_VAR, name, var_type);
  return var_info->addr;
}
