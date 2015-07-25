#include "file.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>

namespace lib {
namespace file {

std::FILE* FopenOrDie(const char* filename, const char* mode) {
  auto* f = std::fopen(filename, mode);
  if (!f) {
    std::cerr << "Error opening file " << filename << std::endl;
    std::exit(1);
  }
  return f;
}

std::string ReadFile(const std::string& input_file) {
  std::ifstream fin(input_file);
  std::string s;
  if (fin) {
    fin.seekg(0, fin.end);
    auto length = fin.tellg();
    s.resize(length);
    fin.seekg(0, fin.beg);
    fin.read(&s[0], s.size());
  }
  return s;
}

}  // namespace file
}  // namespace lib
