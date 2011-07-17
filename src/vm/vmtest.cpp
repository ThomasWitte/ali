#include "vm.hpp"
#include "assembler.hpp"

int main(int argc, char **argv) {
    vm the_vm;
    if(argc == 2) {
        the_vm.load_binary(argv[1]);
    } else if(argc == 3 && std::string(argv[1]) == "-a") {
        std::cout << "Assembling " << argv[2] << std::endl;
        assembler a;
        a.load(argv[2]);
        a.binary_dump(std::string(argv[2]) + ".ali");
        return 0;
    } else {
        std::cout << "Usage: vmtest PROGRAM\n" <<
                     "       vmtest -a ASMFILE\n" <<
                     "Nothing to do." << std::endl;
        return 0;
    }

    the_vm.start();

    return 0;
}
