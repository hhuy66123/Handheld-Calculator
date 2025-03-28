#ifndef POSTFIX_H
#define POSTFIX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAX 100

typedef enum {S_START, S_OPERAND, S_OPERATOR, S_OPEN, S_CLOSE, S_ERROR, S_END} state_t;

typedef enum {
    OPERAND,
    OPERATOR,
    VARIABLE
} TokenType;

typedef struct {
    TokenType type;
    union {
        float operand;
        char op;
        float variable;
    } value;
} Token;

// Hàm chuyển đổi từ infix sang postfix
Token *infixToPostfix(char* myFunction);

// Hàm in các token
void printTokens(Token *output);

// Hàm đánh giá biểu thức postfix tại một giá trị x
float evaluatePostfix(Token *postfix, float x);
#endif // POSTFIX_H
