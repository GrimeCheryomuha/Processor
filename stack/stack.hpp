#pragma once
#include <iostream>
#include <cstring>

#define CANL 0xDEADBEEF 
#define CANR 0xD34DB33F 

#define MULT 37u

#define POISON1 0xBD 
#define POISON2 0xBDCF 
#define POISON4 0xBADC0FEE 
#define POISON8 0xBADC0FEEF04DED32 

#define setPoison(var) setPoisonInside (&var, sizeof (var)) 
#define isPoison(var) isPoisonInside (&var, sizeof (var)) 

void setPoisonInside    (void* var, size_t sizeofVar);
bool isPoisonInside     (void* var, size_t sizeofVar);

int StackGetHash        (char *ptr, size_t len);


using namespace std;

class Stack {

private:

    unsigned int    canL = CANL;
    unsigned int    canR = CANR;

    unsigned int    hash; 
    unsigned int    stack_hash; 

    size_t          size;    
    size_t          capacity;        
    size_t          errCode;

    double*         stack;
    unsigned int*   stackCanL;       
    unsigned int*   stackCanR;       

public:

    Stack (size_t _capacity);

    void StackCountHash                 (void); 
    void StackVerifyHash                (void);

    void    StackPush                   (double val);
    double  StackPop                    (void); 

    void ReallocStackSizeShift          (int delta);
    unsigned long long StackErrCheck    (void);

    ~Stack ();
};

enum STACKERRORS {
    ok                   = 0,    
    POISON_ACCESS        = 1<<0, 
    BAD_CAN_L            = 1<<1, 
    BAD_CAN_R            = 1<<2, 
    BAD_STACK_CAN_L      = 1<<3, 
    BAD_STACK_CAN_R      = 1<<4, 
    NULL_STACK_PTR       = 1<<5, 
    NULL_STACK_CAN_L_PTR = 1<<6, 
    NULL_STACK_CAN_R_PTR = 1<<7, 
    WRONG_SIZE           = 1<<8, 
    POISONED_ERRCOD      = 1<<9, 
    WRONG_HASH           = 1<<10 
};  

#define check cout << "ok" << endl