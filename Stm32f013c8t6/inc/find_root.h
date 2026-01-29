#ifndef FINDROOT_H
#define FINDROOT_H

#include "postfix.h"

float derivativePostfix(Token *postfix, float x);

float secondDerivativePostfix(Token *postfix, float x);

void find_isolation_interval(Token *postfix, float *a_out, float *b_out);


#endif
