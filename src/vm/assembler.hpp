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
    template <typename T, typename T2>
    inline void cpy_val(T dest, const T2& val)
        {
            *(T2*)(dest) = val;
        }

    template <typename T>
    inline void load_symbol(int address, const T& val)
        {
            cpy_val(prg + address, val);
            this->address += sizeof(T);
        }

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
