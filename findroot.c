#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "findroot.h"
#include "postfix.h"

double derivativePostfix(Token *postfix, double x) {
    double h = 1e-2;
    double fxh = evaluatePostfix(postfix, x + h);
    double fxh_neg = evaluatePostfix(postfix, x - h);
    return (fxh - fxh_neg) / (2 * h);
}



float secondDerivativePostfix(Token *postfix, float x) {
    float h = 1e-10; 
    return (evaluatePostfix(postfix, x + h) - 2 * evaluatePostfix(postfix, x) + evaluatePostfix(postfix, x - h)) / (h * h);
}

void find_isolation_interval(Token *postfix, double *a_out, double *b_out) {
    double x1 = -100000, x2 = x1 + 1;
    double step = 1.0;
    int found = 0;
    double epsilon = 1e-6;  

    while (x2 <= 100000) {
        double f_x1 = evaluatePostfix(postfix, x1);
        double f_x2 = evaluatePostfix(postfix, x2);

        int x1_valid = !(isinf(f_x1));
        int x2_valid = !(isinf(f_x2));

        if (!x2_valid) {  
            x1 = x2 + step;  
            x2 = x2 + 2*step;
            continue;
        }

        if ((f_x1 > 0 && f_x2 < 0) || (f_x1 < 0 && f_x2 > 0) || fabs(f_x1) < epsilon || fabs(f_x2) < epsilon) {
            *a_out = x1;
            *b_out = x2;
            found = 1;
            break;
        }
        x1 = round(x2 * 1e6) / 1e6;  
        x2 = round((x2 + step) * 1e6) / 1e6;
        if (x2 > 100000 && step == 1.0) {
            x1 = -100000;
            x2 = x1 + 0.1;
            step = 0.1;
        }  
    }

    if (!found) {
        *a_out = *b_out = NAN;  
    }
}


float newtonRaphson(Token *postfix) {
    double a, b;
    find_isolation_interval(postfix, &a, &b);
    if (isnan(a)||isnan(b)) return NAN;
    double x0;
    if (evaluatePostfix(postfix, a) * secondDerivativePostfix(postfix, a) > 0) {
        x0 = a;
    } else if (evaluatePostfix(postfix, b) * secondDerivativePostfix(postfix, b) > 0) {
        x0 = b;
    } else {
        x0 = (a + b) / 2;  
    }

    for (int i = 0; i < 1000000; i++) {
        float f = evaluatePostfix(postfix, x0);
        double df = derivativePostfix(postfix, x0);
        if (fabs(f) < 1e-7) return x0; 
        if(fabs(df) < 1e-10) break;
        x0 = x0 - f / df;
    }
    return NAN;
}

float bisectionMethod(Token *postfix) {
    double a, b;
    find_isolation_interval(postfix, &a, &b);
    if (isnan(a)||isnan(b)) return NAN;
    float c;
    int i;
    for (i = 0; i < 1000000; i++) {
        c = (a + b) / 2;
        float f_c = evaluatePostfix(postfix, c);
        if (fabs(f_c) < 1e-7) return c;
        if (evaluatePostfix(postfix, a) * f_c < 0) b = c;
        else a = c;
    }
    return NAN;
}

float secantMethod(Token *postfix) {
    double a, b;
    find_isolation_interval(postfix, &a, &b);
    if (isnan(a)||isnan(b)) return NAN;

    float x0 = a, x1 = b, x2;
    

    for (int i = 0; i < 1000000; i++) {
        float f_x0 = evaluatePostfix(postfix, x0);
        float f_x1 = evaluatePostfix(postfix, x1);

        if (fabs(f_x1 - f_x0) < 1e-10) return NAN;

        x2 = x1 - f_x1 * (x1 - x0) / (f_x1 - f_x0);
        if (fabs(x2 - x1) < 1e-7) return x2;

        x0 = x1;
        x1 = x2;
    }

    return NAN;
}
