#ifndef __POSCODE_HPP__
#define __POSCODE_HPP__

#include <string>
#include <cstddef>

class Poscode{
  private:
    std::string data;
  public:
    Poscode();
    explicit Poscode(std::string _data);
    char getValue(std::size_t i) const;           // <- const
    const std::string& getData() const;           // <- const
};

#endif
