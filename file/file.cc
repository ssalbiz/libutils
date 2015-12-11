#include "file.h"

#include <string>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <dirent.h>
#include <pwd.h>

using ::std::string;

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

string ReadFile(const string& input_file) {
  std::ifstream fin(input_file);
  string s;
  if (fin) {
    fin.seekg(0, fin.end);
    auto length = fin.tellg();
    s.resize(length);
    fin.seekg(0, fin.beg);
    fin.read(&s[0], s.size());
  }
  return s;
}

DIR* OpenDirOrDie(const string& dirpath) {
  auto* dir_handle = opendir(dirpath.c_str());
  if (dir_handle == nullptr) {
    std::cerr << "Could not open directory:" << dirpath << std::endl;
    exit(1);
  }
  return dir_handle;
}

string GetCurrentHomeDir() {
  const char* homedir = getenv("HOME");
  return homedir != nullptr
    ? string(homedir)
    : string(getpwuid(getuid())->pw_dir);
}

string RealPath(const string& path) {
  char* resolved = realpath(path.c_str(), nullptr);
  if (resolved != nullptr) {
    // string makes a copy.
    string real(resolved);
    free(resolved);
    return real;
  }
  return "";
}

}  // namespace file
}  // namespace lib
