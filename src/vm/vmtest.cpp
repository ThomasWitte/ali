#include "vm.h"

int main(int argc, char **argv) {
    vm the_vm;
    if(argc > 1)
        the_vm.load_binary(argv[1]);
    else {
        std::cout << "Usage: vmtest PROGRAM\nNothing to do." << std::endl;
        return 0;
    }

    the_vm.start();

    return 0;
}
