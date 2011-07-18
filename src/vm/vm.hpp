#ifndef VM_H
#define VM_H

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cstring>

#define INST_TYPE unsigned char
#define INTEGER long long
#define ADDR_TYPE int

class vm {
    public:
        enum {
            NOP,        //0x00
            ALLOC,
            ADD,        //0x02
            AND,
            APPLY,
            APPLY0,
            CONS,
            COPYGLOB,
            DIV,
            DUP,        //0x09
            EVAL,
            EQ,         //0x0b
            GETBASIC,   //0x0c
            GETVEC,
            GEQ,        //0x0e
            GR,         //0x0f
            HALT,       //0x10
            JUMP,       //0x11
            JUMPZ,      //0x12
            LE,         //0x13
            LEQ,        //0x14
            LOADA,      //0x15
            LOADC,      //0x16
            MARK,
            MARK0,
            MOD,
            MUL,
            MKBASIC,    //0x1b
            MKCLOS,
            MKFUNVAL,
            MKVEC,
            MKVEC0,
            NEG,        //0x20
            NEQ,        //0x21
            NIL,
            NOT,        //0x23
            OR,
            PRINT,      //0x25
            PUSHGLOB,
            PUSHLOC,
            RETURN,
            REWRITE,
            POPENV,
            SLIDE,
            SUB,
            STORE,      //0x2d
            TARG,
            UPDATE,
            WRAP
        };

        vm(unsigned int memsize = 1000000);
        ~vm();

        int start();
        char *exception(std::string msg);
        void load_binary(std::string filename);
    private:
        char *mem;
        ADDR_TYPE pc, sp, gp, fp, hp;
        ADDR_TYPE memsz;
};

struct fraction {
    int nominator;
    int denominator;
};

struct cons_cell {
    ADDR_TYPE car;
    ADDR_TYPE cdr;
};

//I introduce this struct to easily manage different object types.
//By looking at the identifier you can restore the type of the object at every time.
//If every object has the same size, heap management becomes easier.
//Problems: mem_obj is 16 bytes large
//          poor performance

//Performance problems can be solved (mostly) by putting data first in mem_obj,
//so you can directly cast the pointer to the type you want.

struct mem_obj {
    union {
        //Integer Object    id=0x01
        INTEGER i;
        //Double Object     id=0x02
        double d;
        //Fraction Object   id=0x03
        fraction f;
        //Cons cell         id=0x04
        cons_cell cons;
        //Pointer Object    id=0x05
        void *ptr;
    } data;
    char id;
};

#endif
