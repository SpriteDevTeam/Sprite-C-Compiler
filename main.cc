#include <fstream>
#include <string>
#include "List"
#include "Scanner.h"
#include "Token.h"
#include "AST.h"

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

  build_AST(list);

  std::printf("Parsing succeed.\n");

  delete list;
  return 0;
}
