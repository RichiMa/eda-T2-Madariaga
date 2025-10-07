#include "poscode.hpp"
#include <utility> 

Poscode::Poscode() : data("") {}
Poscode::Poscode(std::string _data) : data(std::move(_data)) {}
char Poscode::getValue(std::size_t i) const { return data[i]; }
const std::string& Poscode::getData() const { return data; }
