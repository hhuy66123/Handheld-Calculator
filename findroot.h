#ifndef FINDROOT_H
#define FINDROOT_H

#include "postfix.h"

double derivativePostfix(Token *postfix, double x);

float secondDerivativePostfix(Token *postfix, float x);

void find_isolation_interval(Token *postfix, double *a_out, double *b_out);

float newtonRaphson(Token *postfix);

float bisectionMethod(Token *postfix);

float secantMethod(Token *postfix);

#endif