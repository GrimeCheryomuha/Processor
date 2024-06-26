#define COMA ,
DEF_CMD (hlt, 0, 0, {exit (0);})

DEF_CMD (push, 1, 1, {

    stkPush (*arg);
    // ERRORCHECK
})

DEF_CMD (pop, 2, 1, {

    *arg = stkPop ();
    // ERRORCHECK
})

DEF_CMD (in, 3, 0, {

    std::cin >> tmp;

    stkPush(tmp);
    // ERRORCHECK
})

DEF_CMD (out, 4, 0, {

    tmp = stkPop ();
    // ERRORCHECK

    std::cout << tmp << std::endl;
})

DEF_CMD (add, 5, 0, {

    stkPush (stkPop () + stkPop ());
    // ERRORCHECK
})

DEF_CMD (sub, 6, 0, {

    arg1 = stkPop ();
    arg2 = stkPop ();
    stkPush (arg2 - arg1);
    // ERRORCHECK
})

DEF_CMD (mul, 7, 0, {

    stkPush (stkPop () * stkPop ());
    // ERRORCHECK
})

DEF_CMD (div, 8, 0, {

    stkPush (stkPop () / stkPop ());
    // ERRORCHECK
})

DEF_CMD (jmp, 10, 2, {

    if (*arg >= code_size) {

        errors[static_cast<int> (Errors::WRONG_JMP_IP)] += 1;
        return;
    }

    ip = *arg - 1;
    // ERRORCHECK
})

/// @warning В ip++ может быть ошибка, надо чекнуть епта
#define DEF_JMP(name, num, op)                                      \
DEF_CMD (name, num, 2, {                                            \
                                                                    \
    arg1 = stkPop ();                                               \
    arg2 = stkPop ();                                               \
                                                                    \
    /*ERRORCHECK*/                                                  \
    if (!(arg2 op arg1)){                                           \
                                                                    \
        ip++;                                                       \
        break;                                                      \
    }                                                               \
                                                                    \
    /*ERRORCHECK*/                                                  \
    if (*arg >= code_size) {                                        \
                                                                    \
        errors[static_cast<int> (Errors::WRONG_JMP_IP)] += 1;       \
        return;                                                     \
    }                                                               \
                                                                    \
    ip = *arg - 1;                                                  \
})

DEF_JMP (ja, 11,  >)

DEF_JMP (jae, 12, >=)

DEF_JMP (jb , 13,  <)

DEF_JMP (jbe, 14, <=)

DEF_JMP (je, 15, ==)

DEF_JMP (jne, 16, !=)

DEF_CMD (call, 17, 2, {

    // ERRORCHECK

    if (*arg > code_size) {

        errors[static_cast<int> (Errors::WRONG_JMP_IP)] += 1;
        return;
    }

    stkCallPush (*arg);
    // ERRORCHECK

    ip = *arg;
})

DEF_CMD (ret, 18, 2, {

    ip = stkCallPop ();

    // ERRORCHECK
})

DEF_CMD (dump, 19, 0, {

    std::cout << "Processor dump at timestamp " << std::time (NULL) << std::endl;

    printf ("ip = %d\nrax = %d\nrbx = %d\nrcx = %d\nrdx = %d\n", ip, regs[rax], regs[rbx], regs[rcx], regs[rdx]);

    printf ("code window [-10, +10] : [%d", code [std::max (0, ip - 10)]);
    for (int i = std::max (1, ip - 9); i < std::min (code_size, ip + 11); i++) {

        if (i == ip) printf (", {%02hhX}", code[i]);
        else printf (", %02hhX", code[i]);
    }

    printf ("]\n");

    if (stk.size ()) printf ("top of stk : %d\n", stk.top ());
    if (call_stk.size ()) printf ("top of call_stk : %d\n", call_stk.top ());

    printf ("ram :\n");
    int sqrtRam = sqrt (RAM_SIZE);
    for (int i = 0; i < sqrtRam; i++) {

        for (int j = 0; j < sqrtRam; j++) {

            printf ("%8d ", ram[(i * sqrtRam) + j]);
        }
        printf ("\n");
    }

    printf ("End of dump\n\n");
})

#undef DEF_JMP