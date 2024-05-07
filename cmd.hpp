#define FROM_ARG 1
#define TO_ARG 2
#define IP_ARG 3

DEF_CMD (push, 1, FROM_ARG,
{
    double arg = 0;
    getArg (cmd, &arg);

    stk.push (arg);

    if (stk.size() > STACK_CAPACITY) 
        return;
})

DEF_CMD (pop, 2, TO_ARG, 
{
    double arg = 0;
    getArg (cmp, &arg);
    stkPop (&arg);
})

DEF_CMD (add, 3, 0,
{
    double x1, x2;
    stk.puph (stkPop(&x1), stkPop(&x2));
})

DEF_CMD (sub, 4, 0,
{
    double x1, x2;
    stk.push (stkPop(&x1) - stkPop(&x2));
})

DEF_CMD (mul, 5, 0,
{
    double x1, x2;
    stk.push (stkPop(&x1) * stkPop(&x2));
})

DEF_CMD (div, 6, 0,
{
    double x1, x2;
    stk.push (stkPop(&x1) / stk(&x2));
})

DEF_CMD (in, 7, 0,
{
    double x;
    cin >> x;
    stk.push(x);
})

DEF_CMD (out, 8, 0,
{
    double x;
    cout << stkPop(&x) << endl;
})

#undef FROM_ARG
#undef TO_ARG
#undef IP_ARG