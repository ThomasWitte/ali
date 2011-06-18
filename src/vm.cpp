#include "vm.h"

#define MEM(ADDR) \
    (((ADDR) >= 0 && (ADDR) < memsz) ? (mem + (ADDR)) : exception("Segfault"))

#define COPY(DEST, VAL, SIZE)                           \
    if((DEST) >= 0 && (DEST) < memsz &&                 \
        (VAL) >= 0 && (VAL) < memsz) {                  \
        memcpy(mem + (DEST), mem + (VAL), (SIZE));      \
    } else exception("Segfault");

#define INST_TYPE unsigned char
#define INTEGER long long
#define ADDR_TYPE int

#define _JUMPZ()                                        \
    if(*(INTEGER*)MEM(sp) == 0) {                       \
        pc = *(ADDR_TYPE*)(inst + sizeof(INST_TYPE));   \
        sp -= sizeof(INTEGER);                          \
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
    /*move value to the heap*/                          \
    hp -= sizeof(INTEGER);                              \
    COPY(hp, sp, sizeof(INTEGER));                      \
    sp -= sizeof(INTEGER);                              \
    /*write an identifier to hp;*/                      \
    --hp;                                               \
    *MEM(hp) = 'B';                                     \
    /*push a pointer to the stack*/                     \
    *(ADDR_TYPE*)MEM(sp) = hp;                          \
    sp += sizeof(ADDR_TYPE);                            \
    break;

vm::vm(unsigned int memsize) {
    mem = NULL;
    pc = sp = gp = fp = 0;
    hp = memsize-1;
    memsz = memsize;

    if(memsize <= 0 || !(mem = new char[memsize])) {
        std::cerr << "Failed to initialize VM." << std::endl;
        exit(EXIT_FAILURE);
    }

    //Load Program
    INST_TYPE *prog = (INST_TYPE*)MEM(pc);

    char p[] = {
        NOP,    LOADC,  0,      0,      0,      0,      0,      0,
        0,      0,      JUMPZ,  16,     0,      0,      0,      NIL,
        LOADC,  5,      0,      0,      0,      0,      0,      0,
        0,      MKBASIC,HALT
    };
    memcpy(prog, p, 27);
    sp = 27;
}

vm::~vm() {
    if(mem)
        delete [] mem;
}

int vm::start() {
    if(!mem)
        return -1;

    for(;;) {
        //I'm not sure about the size of the pointers
        char *inst = MEM(pc);
        std::cout << (int)*inst << std::endl;

        switch(*inst) {
            case NOP:
            break;

            case HALT:
            return 0;

            case JUMP:
            pc = *(ADDR_TYPE*)(inst + sizeof(INST_TYPE));
            continue;

            case JUMPZ:
            //Jumps if the long long on top of the stack is 0
            _JUMPZ();

            case LOADC:
            //loads a INTEGER constant on the stack
            _LOADC();

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

char *vm::exception(std::string msg) {
    //This should also show the values and a stackdump/memory dump

    std::cerr << "Error: " << msg << std::endl;
    std::cerr << "> PC = " << pc << std::endl;
    std::cerr << "> SP = " << sp << std::endl;
    std::cerr << "> GP = " << gp << std::endl;
    std::cerr << "> FP = " << fp << std::endl;
    return mem;
}
