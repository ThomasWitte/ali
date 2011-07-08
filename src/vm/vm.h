#ifndef VM_H
#define VM_H

#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>

class vm {
    public:
        enum {
            NOP,
            ALLOC,
            ADD,
            AND,
            APPLY,
            APPLY0,
            CONS,
            COPYGLOB,
            DIV,
            EVAL,
            EQ,
            GETBASIC,
            GETVEC,
            GEQ,
            GR,
            HALT,
            JUMP,
            JUMPZ,
            LE,
            LEQ,
            LOADC,
            MARK,
            MARK0,
            MOD,
            MUL,
            MKBASIC,
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
            TARG,
            UPDATE,
            WRAP
        };

        vm(unsigned int memsize = 1000000);
        ~vm();

        int start();
        char *exception(std::string msg);
    private:
        char *mem;
        unsigned int pc, sp, gp, fp, hp;
        unsigned int memsz;
};

#endif
