#ifndef __POSCODE_HPP__
#define __POSCODE_HPP__

#include <string>
#include <cstddef>

class Poscode{
  private:
    std::string data;
  public:
    Poscode(): data("") {}
    explicit Poscode(std::string _data): data(std::move(_data)) {}

    // acceso por posición (0..5)
    char getValue(std::size_t i) const { return data[i]; }
    const std::string& getData() const { return data; }
};

#endif
