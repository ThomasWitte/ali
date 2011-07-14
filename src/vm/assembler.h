#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <string>
#include <sstream>

class assembler {
    public:
        assembler();
        ~assembler();
        void load(std::string filename);
        void binary_dump(std::string filename);
    private:
        char *prg;
        int address;
};

template <class T>
    T from_string(std::string Converted)
    {
        std::stringstream buffer;
        T ret;
    	buffer << Converted;
        buffer >> ret;
    	return ret;
    }

#endif
