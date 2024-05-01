#include "stack.hpp"


void setPoisonInside (void* var, size_t sizeofVar) {

    if (var == NULL) return;

    switch(sizeofVar) {
        case 1 : *((unsigned char*     ) var)      = POISON1; break;
        case 2 : *((unsigned short*    ) var)      = POISON2; break;
        case 4 : *((unsigned int*      ) var)      = POISON4; break;
        case 8 : *((unsigned long long*) var)      = POISON8; break;

        default:
            memset (var, POISON1, sizeofVar);
        break;
    }
}


bool isPoisonInside (void* var, size_t sizeofVar) {

    if (var == NULL) return 1;

    switch(sizeofVar) {
        case 1:
            if ( *( (unsigned char*     ) var) == POISON1) return 1;
            else                                           return 0;
            break;
        case 2:
            if ( *( (unsigned short*    ) var) == POISON2) return 1;
            else                                           return 0;
            break;
        case 4:
            if ( *( (unsigned int*      ) var) == POISON4) return 1;
            else                                           return 0;
            break;
        case 8:
            if ( *( (unsigned long long*) var) == POISON8) return 1;
            else                                           return 0;
            break;
        default:
            for (int i = 0; i < sizeofVar; i++) {

                if (*( ( (unsigned char*) var) + i) != POISON1) return 0;
            }
            return 1;
            break;
    }
}

int StackGetHash (char *ptr, size_t len) {

    int hash = 3571;

    for (int i = 0; i < len; i++) 
        hash = (hash << 5) + hash + *(ptr + i);

    return hash;
}


Stack::Stack (size_t _capacity) {
    
    capacity = _capacity;
    size = 0;

    stack = reinterpret_cast<double*> (new char[capacity * sizeof (double) + 2 * sizeof (unsigned int)] + sizeof (unsigned int));
    stackCanL = reinterpret_cast<unsigned int*> (stack);
    stackCanR = reinterpret_cast<unsigned int*> (stack + capacity);

    *stackCanL = CANL;
    *stackCanR = CANR;

    
    StackCountHash ();
}

Stack::~Stack () {

    delete[] reinterpret_cast<char*>(stackCanL - 1);
}

void        Stack::StackCountHash (void) {

    hash = 0;

    stack_hash  = StackGetHash (reinterpret_cast<char*> (stack) - sizeof (unsigned int), capacity * sizeof (stack[0]) + 2 * sizeof (unsigned int)); 
    hash        = StackGetHash (reinterpret_cast<char*> (this), sizeof (*this));  
}


void        Stack::StackPush        (double val) {

    StackVerifyHash ();

    if (errCode != ok) return;

    StackCountHash ();

    if (size == capacity) ReallocStackSizeShift (1);

    stack[size] = val;
    size++;

    StackErrCheck   ();

    StackCountHash  ();
}

double      Stack::StackPop         (void) {

    StackVerifyHash ();
    StackErrCheck   ();

    if (errCode != ok) return 0;
    if (size == 0) return 0; 
    double retVal = stack[size - 1];
    stack[size-1] = 0;
    size--;

    StackCountHash ();

    if (capacity > 4 and size == capacity * 3 / 8) ReallocStackSizeShift (-1);

    StackErrCheck  ();

    StackCountHash ();

    return retVal;
}

void        Stack::ReallocStackSizeShift (int delta) {

    StackVerifyHash ();
    StackErrCheck ();
    if (errCode != ok) return;

   *((unsigned int*) (stack + capacity)) = 0;

    if (delta > 0) {

        stack = new double[capacity * 2];
        memcpy (stack, stackCanL, capacity * sizeof (double) + 2 * sizeof (unsigned int)); 
        capacity *= 2;

    }
    else if (delta < 0) {

        stack = new double[capacity / 2];
        memcpy (stack, stackCanL, capacity * sizeof (double) / 2 + 2 * sizeof (unsigned int));
        capacity /= 2;
    }

    delete stackCanL;

    stackCanL = (unsigned int*) stack;

    stackCanL = reinterpret_cast<unsigned int*> (stack);
    stackCanR = reinterpret_cast<unsigned int*> (stack + capacity);
    *stackCanR = CANR;

    StackErrCheck ();

    StackCountHash ();
}

void        Stack::StackVerifyHash  (void) {

    unsigned int oldHash = hash;
    StackCountHash ();

    if (hash != oldHash) errCode |= WRONG_HASH;

    return;
}

unsigned long long Stack::StackErrCheck (void) {

    if (isPoison (errCode)) {
        errCode = POISONED_ERRCOD;
        return errCode;
    }

    if (isPoison ( canL     ) or
        isPoison ( canR     ) or
        isPoison ( capacity ) or
        isPoison ( size     ) or
        isPoison ( stack    ) or
        isPoison ( stackCanL) or
        isPoison (*stackCanL) or
        isPoison ( stackCanR) or
        isPoison (*stackCanR) or
        isPoison ( hash     )   ) errCode |= POISON_ACCESS;
    else {

        for (int i = 0; i < capacity;i++) {

            if (isPoison (stack[i])) {

                errCode |= POISON_ACCESS;
                break;
            }
        }
    }

    if ( canL      != CANL         ) errCode |= BAD_CAN_L;
    if ( canR      != CANR         ) errCode |= BAD_CAN_R;
    if ( stackCanL == NULL         ) errCode |= NULL_STACK_CAN_L_PTR;
    if ( stackCanR == NULL         ) errCode |= NULL_STACK_CAN_R_PTR;
    if (*stackCanL != CANL         ) errCode |= BAD_STACK_CAN_L;
    if (*stackCanR != CANR         ) errCode |= BAD_STACK_CAN_R;
    if ( stack     == NULL         ) errCode |= NULL_STACK_PTR;
    if ( size      >  capacity     ) errCode |= WRONG_SIZE;

    StackCountHash ();

    return errCode;
}

