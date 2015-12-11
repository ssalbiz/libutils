#pragma once
#ifndef LIB_FILE_H_
#define LIB_FILE_H_

#include <string>
#include <dirent.h>

/**
 * Simple utility functions for dealing with files
 */
namespace lib {
namespace file {

std::FILE* FopenOrDie(const char* filename, const char* mode);

std::string ReadFile(const std::string& input_file);

DIR* OpenDirOrDie(const std::string& dirpath);

// Pass a callable that takes as its argument a struct dirent*
template <class Func>
void VisitDirEntries(DIR* dir_handle, Func f) {
  auto* dir_entry = readdir(dir_handle);
  while (dir_entry != nullptr) {
    f(dir_entry);
    dir_entry = readdir(dir_handle);
  }
}

std::string GetCurrentHomeDir();

std::string RealPath(const std::string& path);

}  // namespace file
}  // namespace lib

#endif
