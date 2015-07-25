#pragma once
#ifndef LIB_FILE_H_
#define LIB_FILE_H_

#include <string>

/**
 * Simple utility functions for dealing with files
 */
namespace lib {
namespace file {

std::FILE* FopenOrDie(const char* filename, const char* mode);

std::string ReadFile(const std::string& input_file);

}  // namespace file
}  // namespace lib

#endif
