#include "vm.hpp"

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
    *(INTEGER*)FMEM(sp - 2*sizeof(mem_obj)) +=          \
        *(INTEGER*)FMEM(sp - sizeof(mem_obj));          \
    sp -= sizeof(mem_obj);                              \
    break;

#define _DUP()                                          \
    *(mem_obj*)MEM(sp) =                                \
        *(mem_obj*)FMEM(sp - sizeof(mem_obj));          \
    sp += sizeof(mem_obj);                              \
    break;

#define _GETBASIC()                                     \
    /*save the address in addr1*/                       \
    /*FMEM seems to be slower, I don't know why…*/      \
    addr1 = *(ADDR_TYPE*)MEM(sp - sizeof(ADDR_TYPE));   \
    sp -= sizeof(ADDR_TYPE);                            \
    /*get the value (skip one byte for the identifier)*/\
    COPY(sp, addr1, sizeof(mem_obj));                   \
    sp += sizeof(mem_obj);                              \
    break;

#define _JUMPZ()                                        \
    sp -= sizeof(mem_obj);                              \
    if(*(INTEGER*)FMEM(sp) == 0) {                      \
        pc = *(ADDR_TYPE*)(inst + sizeof(INST_TYPE));   \
        continue;                                       \
    }                                                   \
    /*Skip the argument of jumpz*/                      \
    pc += sizeof(ADDR_TYPE);                            \
    break;

#define _LOADA()                                        \
    COPY(sp,                                            \
         sp+*(ADDR_TYPE*)FMEM(pc+sizeof(INST_TYPE)),    \
         sizeof(ADDR_TYPE));                            \
    sp += sizeof(ADDR_TYPE);                            \
    pc += sizeof(ADDR_TYPE);                            \
    break;

#define _LOADC()                                        \
    COPY(sp, pc+sizeof(INST_TYPE), sizeof(mem_obj));    \
    sp += sizeof(mem_obj);                              \
    pc += sizeof(mem_obj);                              \
    break;

#define _MKBASIC()                                      \
    /*allocate memory on the heap*/                     \
    NEW(addr1, sizeof(mem_obj));                        \
    /*copy the data*/                                   \
    sp -= sizeof(mem_obj);                              \
    COPY(addr1, sp, sizeof(mem_obj));                   \
    /*push a pointer to the stack*/                     \
    *(ADDR_TYPE*)MEM(sp) = addr1;                       \
    sp += sizeof(ADDR_TYPE);                            \
    break;

#define _PRINT()                                        \
    switch(((mem_obj*)FMEM(sp - sizeof(mem_obj)))->id) {\
        case 0x01: /*INTEGER*/                          \
        std::cout <<                                    \
         ((mem_obj*)FMEM(sp - sizeof(mem_obj)))->data.i \
         << std::endl;                                  \
        break;                                          \
                                                        \
        case 0x02: /*Double*/                           \
        std::cout <<                                    \
         ((mem_obj*)FMEM(sp - sizeof(mem_obj)))->data.d \
         << std::endl;                                  \
        break;                                          \
                                                        \
        case 0x03: /*Fraction*/                         \
        std::cout <<                                    \
         ((mem_obj*)FMEM(sp - sizeof(mem_obj)))         \
             ->data.f.nominator << "/" <<               \
         ((mem_obj*)FMEM(sp - sizeof(mem_obj)))         \
             ->data.f.denominator                       \
         << std::endl;                                  \
        break;                                          \
                                                        \
        case 0x04: /*Cons-Cell*/                        \
        if(((mem_obj*)FMEM(sp - sizeof(mem_obj)))       \
            ->data.cons.cdr == 0) {                     \
            /*Cons-cell is not a list*/                 \
            std::cout << "(" <<                         \
             ((mem_obj*)FMEM(sp - sizeof(mem_obj)))     \
                 ->data.cons.car << " . " <<            \
             ((mem_obj*)FMEM(sp - sizeof(mem_obj)))     \
                 ->data.cons.cdr << ")"                 \
             << std::endl;                              \
        } else {                                        \
            /*TODO: Print list*/                        \
        }                                               \
        break;                                          \
                                                        \
        case 0x05: /*Pointer*/                          \
            std::cout <<                                \
             ((mem_obj*)FMEM(sp - sizeof(mem_obj)))     \
                 ->data.ptr                             \
             << std::endl;                              \
        break;                                          \
                                                        \
        default:                                        \
            exception("PRINT: Not an Object");          \
        break;                                          \
    }                                                   \
    sp -= sizeof(mem_obj);                              \
    break;

#define _STORE()                                        \
    COPY(*(ADDR_TYPE*)FMEM(                             \
            sp - sizeof(ADDR_TYPE)),                    \
         sp - sizeof(mem_obj) - sizeof(ADDR_TYPE),      \
         sizeof(mem_obj));                              \
    /*adjust the stack pointer*/                        \
    sp -= sizeof(ADDR_TYPE) + sizeof(mem_obj);          \
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

            case LOADA:
            //loads the address, the argument points to on top of the stack
            _LOADA();

            case LOADC:
            //loads a INTEGER constant on the stack
            _LOADC();

            case PRINT:
            //Prints the mem_obj on the stack to stdout
            _PRINT();

            case STORE:
            //pushes the value at pos 2 of the stack to the adress lying on top of the stack
            //there is no check, if the destination address is allocated or of the right type
            _STORE();

            case MKBASIC:
            //Takes the mem_obj from the stack and stores it on the heap
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
