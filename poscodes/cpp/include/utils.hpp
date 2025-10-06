#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include "poscode.hpp"
#include <string>
#include <cstddef>

void quick_sort(Poscode *A, std::size_t n);
void merge_sort(Poscode *A, std::size_t n);
void radix_sort(Poscode *A, std::size_t n);

Poscode *readCodes(const std::string &strfile, std::size_t n);
void deleteCodes(Poscode *codes);

#endif
