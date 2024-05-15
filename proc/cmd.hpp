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