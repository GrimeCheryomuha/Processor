DEF_CMD (push, 1, 1, {

    int arg = 0;
    getArgs (cmd, &arg);

    stkPush (arg);
    ERRORCHECK
})

DEF_CMD (pop, 2, 1, {

    int arg = 0;
    getArgs (cmd, &arg);

    arg = stkPop ();
    ERRORCHECK
})

DEF_CMD (in, 3, 0, {

    int arg = 0;
    cin >> arg;

    stkPush(arg);
    ERRORCHECK
})

DEF_CMD (out, 4, 0, {

    int arg = 0;
    
    arg = stk.Pop ();
    ERRORCHECK

    cout << arg << endl;
})

DEF_CMD (add, 5, 0, { 
    
    stkPush (stkPop () + stkPop ());
    ERRORCHECK
})

DEF_CMD (sub, 6, 0, { 
    
    stkPush (stkPop () - stkPop ());
    ERRORCHECK
})

DEF_CMD (mul, 7, 0, { 
    
    stkPush (stkPop () * stkPop ());
    ERRORCHECK
})

DEF_CMD (div, 8, 0, { 
    
    stkPush (stkPop () / stkPop ());
    ERRORCHECK
})

#define DEF_JMP(name, num, operator)                                \
DEF_CMD (name, num, 1, {                                            \
                                                                    \
    int arg1 = 0, arg2 = 0;                                         \
    arg1 = stkPop ();                                               \
    arg2 = stkPop ();                                               \
                                                                    \
    ERRORCHECK                                                      \
    if (!(arg2 op arg1)){                                           \
                                                                    \
        ip++;                                                       \
        break;                                                      \
    }                                                               \
                                                                    \
    int arg = 0;                                                    \
    getArgs (cmd, &arg);                                            \
                                                                    \
    ERRORCHECK                                                      \
    if (arg >= code_size) {                                         \
                                                                    \
        errors[static_cast<int> (Errors::WRONG_JMP_IP)] += 1;       \
        return;                                                     \
    }                                                               \
                                                                    \
    ip = arg;                                                       \
})                                                                  

DEF_JMP (ja, 9,  >)

DEF_JMP (jae, 10, >=)

DEF_JMP (jb , 11,  <)

DEF_JMP (jbe, 12, <=)

DEF_JMP (je, 13, ==)

DEF_JMP (jne, 14, !=)