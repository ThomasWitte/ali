#include "vm.h"

#define INST_TYPE unsigned char
#define INTEGER long long
#define ADDR_TYPE unsigned int


//returns the real address corresponding to an address in the vm
//returns calls exception(string) if ADDR is not valid
#define MEM(ADDR) \
    (((ADDR) >= 0 && (ADDR) < memsz) ? (mem + (ADDR)) : exception("Segfault"))

//faster version of MEM without address check
#define FMEM(ADDR) (mem + (ADDR))

//copies SIZE bytes from VAL to DEST
//calls exception if one of the addresses is not valid
#define COPY(DEST, VAL, SIZE)                           \
    if((DEST) >= 0 && (DEST) < memsz &&                 \
        (VAL) >= 0 && (VAL) < memsz) {                  \
        memcpy(FMEM(DEST), FMEM(VAL), (SIZE));          \
    } else exception("Segfault");

//allocates SIZE bytes on the heap and returns the address in PTR
#define NEW(PTR, SIZE)                                  \
    hp -= (SIZE);                                       \
    PTR = hp;

//frees PTR (currently it does nothing)
#define FREE(PTR)


//implementations of the instructions

#define _ADD()                                          \
    *(INTEGER*)FMEM(sp - 2*sizeof(INTEGER)) +=          \
        *(INTEGER*)FMEM(sp - sizeof(INTEGER));          \
    sp -= sizeof(INTEGER);                              \
    break;

#define _DUP()                                          \
    *(INTEGER*)MEM(sp) =                                \
        *(INTEGER*)FMEM(sp - sizeof(INTEGER));          \
    sp += sizeof(INTEGER);                              \
    break;

#define _GETBASIC()                                     \
    /*save the address in addr1*/                       \
    addr1 = *(ADDR_TYPE*)MEM(sp - sizeof(ADDR_TYPE));   \
    sp -= sizeof(ADDR_TYPE);                            \
    /*get the value (skip one byte for the identifier)*/\
    COPY(sp, addr1+1, sizeof(INTEGER));                 \
    sp += sizeof(INTEGER);                              \
    break;

#define _JUMPZ()                                        \
    sp -= sizeof(INTEGER);                              \
    if(*(INTEGER*)FMEM(sp) == 0) {                      \
        pc = *(ADDR_TYPE*)(inst + sizeof(INST_TYPE));   \
        continue;                                       \
    }                                                   \
    /*Skip the argument of jumpz*/                      \
    pc += sizeof(ADDR_TYPE);                            \
    break;

#define _LOADC()                                        \
    COPY(sp, pc+sizeof(INST_TYPE), sizeof(INTEGER));    \
    sp += sizeof(INTEGER);                              \
    pc += sizeof(INTEGER);                              \
    break;

#define _MKBASIC()                                      \
    /*allocate memory on the heap*/                     \
    /*we need one byte more for the type-indentifier*/  \
    NEW(addr1, sizeof(INTEGER) + 1);                    \
    /*write an identifier to addr1*/                    \
    *MEM(addr1) = 'B';                                  \
    /*copy the data*/                                   \
    sp -= sizeof(INTEGER);                              \
    COPY(addr1+1, sp, sizeof(INTEGER));                 \
    /*push a pointer to the stack*/                     \
    *(ADDR_TYPE*)MEM(sp) = addr1;                       \
    sp += sizeof(ADDR_TYPE);                            \
    break;

#define _STORE()                                        \
    COPY(sp - sizeof(INTEGER),                          \
/*this is the destination address + 1 byte identifier*/ \
         *(ADDR_TYPE*)MEM(                              \
            sp - sizeof(INTEGER) - sizeof(ADDR_TYPE))+1,\
         sizeof(INTEGER));                              \
    /*adjust the stack pointer*/                        \
    sp -= sizeof(ADDR_TYPE) + sizeof(INTEGER);          \
    break;

vm::vm(unsigned int memsize) {
    //initialize the vm
    mem = NULL;
    pc = sp = gp = fp = 0;
    hp = memsize-1;
    memsz = memsize;

    if(memsize <= 0 || !(mem = new char[memsize])) {
        std::cerr << "Failed to initialize VM." << std::endl;
        exit(EXIT_FAILURE);
    }
}

vm::~vm() {
    if(mem)
        delete [] mem;
}

void vm::load_binary(std::string filename) {
    std::ifstream file;
    file.open(filename.c_str(), std::ios_base::in);
    if(!file) {
        std::cerr << "Could not open file " << filename << std::endl;
        exit(EXIT_FAILURE);
    }

    int i = 0;
    for(; !file.eof(); ++i) {
        file.read(&mem[i], 1);
    }

    file.close();

    sp = i+1;
    pc = fp = gp = 0;
    hp = memsz-1;
    std::cout << "Starting..." << std::endl;
}

int vm::start() {
    if(!mem)
        return -1;

    //some temporary addresses some instructions might need
    ADDR_TYPE addr1;

    for(;;) {
        //load next instruction
        char *inst = MEM(pc);

        //prints the current instruction
        //just for debugging purposes
        //std::cout << (int)*inst << std::endl;

        switch(*inst) {
            case NOP:
            break;

            case ADD:
            //adds the two integers on top of the stack
            _ADD();

            case DUP:
            //duplicates the integer on top of the stack
            _DUP();

            case GETBASIC:
            //replaces a pointer to a data object with the object
            _GETBASIC();

            case HALT:
            //Stops the vm
            return 0;

            case JUMP:
            //Jumps to an address
            pc = *(ADDR_TYPE*)(inst + sizeof(INST_TYPE));
            continue;

            case JUMPZ:
            //Jumps if the long long on top of the stack is 0
            _JUMPZ();

            case LOADC:
            //loads a INTEGER constant on the stack
            _LOADC();

            case STORE:
            //pushes the value on top of the stack to the adress lying at pos 2 of the stack
            //there is no check, if the destination address is allocated or of the right type
            _STORE();

            case MKBASIC:
            //Takes the INTEGER from the stack and stores it on the heap
            _MKBASIC();

            default:
            std::cerr << "Unrecognized Opcode " << (int)*inst << std::endl;
        }

        pc += sizeof(INST_TYPE);
    }

    return 0;
}

//called if an instruction encounters an error
char *vm::exception(std::string msg) {
    //This should also show the values and a stackdump/memory dump

    std::cerr << "Error: " << msg << std::endl;
    std::cerr << "> PC = " << pc << std::endl;
    std::cerr << "> SP = " << sp << std::endl;
    std::cerr << "> GP = " << gp << std::endl;
    std::cerr << "> FP = " << fp << std::endl;
    exit(EXIT_FAILURE);
    return mem;
}
