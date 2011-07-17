#ifndef VM_H
#define VM_H

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cstring>

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
            EQ,
            GETBASIC,   //0x0c
            GETVEC,
            GEQ,
            GR,
            HALT,       //0x10
            JUMP,       //0x11
            JUMPZ,      //0x12
            LE,
            LEQ,
            LOADC,      //0x15
            MARK,
            MARK0,
            MOD,
            MUL,
            MKBASIC,    //0x1a
            MKCLOS,
            MKFUNVAL,
            MKVEC,
            MKVEC0,
            NEG,
            NEQ,
            NIL,
            OR,
            PUSHGLOB,
            PUSHLOC,
            RETURN,
            REWRITE,
            POPENV,
            SLIDE,
            SUB,
            STORE,      //0x2a
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
        unsigned int pc, sp, gp, fp, hp;
        unsigned int memsz;
};

#endif
