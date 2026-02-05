#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "find_root.h"
#include "postfix.h"
#include "cmsis_os.h"

float derivativePostfix(Token *postfix, float x) {
    float h = 1e-2;
    float fxh = evaluatePostfix(postfix, x + h);
    float fxh_neg = evaluatePostfix(postfix, x - h);
    return (fxh - fxh_neg) / (2 * h);
}


float secondDerivativePostfix(Token *postfix, float x) {
    float h = 1e-10;
    return (evaluatePostfix(postfix, x + h) - 2 * evaluatePostfix(postfix, x)
            + evaluatePostfix(postfix, x - h)) / (h * h);
}


void find_isolation_interval(Token *postfix, float *a_out, float *b_out) {
    float x1 = -20, x2 = x1 + 1;
    float step = 1.0;
    int found1 = 0;
    float epsilon = 1e-5;

    while (x2 <= 20) {

        //pthread_testcancel();

        float f_x1 = evaluatePostfix(postfix, x1);
        float f_x2 = evaluatePostfix(postfix, x2);
			 float f_x11 = fabs(f_x1);
			
				/*SSD1306_Clear();
        printString(0, 0, "x1=");
        printFloat(x1, 35, 0);
        printString(0, 10, "x2=");
        printFloat(x2, 35, 10);
        printString(0, 20, "f(x1)=");
        printFloat(f_x11, 45, 20);
        printString(0, 30, "f(x2)=");
        printFloat(f_x2, 45, 30);
        SSD1306_UpdateScreen();
        HAL_Delay(10);*/

        int x1_valid = !(isinf(f_x1));
        int x2_valid = !(isinf(f_x2));

        if (!x2_valid) {
            x1 = x2 + step;
            x2 = x2 + 2 * step;
            continue;
        }

         if (((f_x1 > 0 && f_x2 < 0) && (!isnan(f_x1) && !isnan(f_x2))) ||
    ((f_x1 < 0 && f_x2 > 0) && (!isnan(f_x1) && !isnan(f_x2))) ||
    fabs(f_x1) < epsilon || fabs(f_x2) < epsilon) {
    *a_out = x1;
    *b_out = x2;
    found1 = 1;
    break;
}

        x1 = round(x2 * 1e6) / 1e6;
        x2 = round((x2 + step) * 1e6) / 1e6;
        if (x2 > 20 && step == 1.0) {
            x1 = -20;
            x2 = x1 + 0.1;
            step = 0.1;
        }
    }

    if (!found1) {
        *a_out = *b_out = NAN;
    }
}

