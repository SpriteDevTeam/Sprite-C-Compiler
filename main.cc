#include <fstream>
#include <string>
#include "CST.h"
#include "List.h"
#include "Parser.h"
#include "Scanner.h"
#include "SymbolTable.h"
#include "Token.h"
#include "CodeGen.h"

int main(int argc, char* argv[]) {
  // maybe some checks to argc and argv
  std::fstream file(std::string(argv[1]), std::fstream::in);
  if (file.fail()) {
    std::fprintf(stderr, "File Error: \"%s\" not existed.\n", argv[1]);
    return -1;
  }

  List<Token>* list = new List<Token>();
  scanner(list, file);
  file.close();
  check_scan_error(list);

  //list->display();

  CSTNode* root = build_CST(list);

  std::printf("Parsing succeed.\n");

  // std::cout << "\n--- Original CST ---" << std::endl << std::endl;
  // display_CST(root);

  flatten_CST(root);
  // std::cout << "\n--- Flattened CST ---" << std::endl << std::endl;
  // display_CST(root);

  graft_CST(root);
  // std::cout << "\n--- Grafted CST ---" << std::endl << std::endl;
  // display_CST(root);

  fix_CST(root);
  // std::cout << "\n--- Fix left-associative CST ---" << std::endl << std::endl;
  // display_CST(root);

  trim_CST(root);
  std::cout << "\n--- Trimmed CST ---" << std::endl << std::endl;
  display_CST(root);

  auto st = build_symbol_table(root);
  display_symbol_table(st);

  gen_code(st);

  destruct_CST(root);
  delete list;
  return 0;
}
