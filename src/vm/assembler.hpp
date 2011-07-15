#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <string>
#include <sstream>

class assembler {
public:
    assembler();
    ~assembler();
    void load(const std::string& filename);
    void binary_dump(const std::string& filename);
private:
    char *prg;
    int address;
};

template <class T>
T from_string(const std::string& str)
{
    std::stringstream buffer;
    T ret;
    buffer << str;
    buffer >> ret;
    return ret;
}

#endif
