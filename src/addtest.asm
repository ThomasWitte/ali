    loadc 0
    mkbasic
    loadc 100000000
a:  dup
    jumpz b
    dup
    loada #-36
    getbasic
    add
    loada #-36
    store
    loadc -1
    add
    jump a
b:  loada #-20
    getbasic
    print
    halt
