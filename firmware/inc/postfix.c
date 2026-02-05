#include "postfix.h"
#include <string.h> // Thêm d? dùng strncmp

int minus = 0;
int openParentheses = 0; // Bi?n d?m d?u ngo?c m?

int isOperator(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/' || c == '^');
}

int precedence(char op) {
    switch(op) {
        case '+':
        case '-': return 1;
        case '*':
        case '/': return 2;
        case '^': return 3;
        default: return 0;
    }
}

Token *infixToPostfix(char* myFunction) {
    state_t current_state = S_START;
    static Token output[MAX];  // B? d?m tinh cho bi?u th?c h?u t?
    int outputIndex = 0;
    char stack[MAX];
    int stackTop = -1;

    while (1) {
        switch (current_state) {
            case S_START:
                if (isdigit(*myFunction) || *myFunction == '.' || *myFunction == 'x') {
                    current_state = S_OPERAND;
                } else if (*myFunction == '(') {
                    openParentheses++; // Tang bi?n d?m d?u ngo?c
                    current_state = S_OPEN;
                } else if (*myFunction == 0) {
                    current_state = S_END;
                } else if (strncmp(myFunction, "cos", 3) == 0) {
                    stack[++stackTop] = 'c'; // Ðánh d?u hàm cos
                    myFunction += 3; // B? qua "cos"
                    if (*myFunction == '(') {
                        openParentheses++;
                        current_state = S_OPEN;
                    } else {
                        current_state = S_ERROR;
                    }
                } else if (strncmp(myFunction, "sin", 3) == 0) {
                    stack[++stackTop] = 's'; // Ðánh d?u hàm sin
                    myFunction += 3; // B? qua "sin"
                    if (*myFunction == '(') {
                        openParentheses++;
                        current_state = S_OPEN;
                    } else {
                        current_state = S_ERROR;
                    }
                } else if (isOperator(*myFunction)) {
                    current_state = S_OPERATOR;
                    if (*myFunction == '-') {
                        minus = 1;  
                        current_state = S_OPERAND;
                        myFunction++;
                    }
                } else {
                    current_state = S_ERROR;
                }
                break;

            case S_OPERAND: {
                float operand = 0.0;
                int decimal_flag = 0;
                float decimal_divisor = 1.0;
                
                if (*myFunction == '-') {
                    myFunction++;
                    output[outputIndex].type = OPERAND;
                    output[outputIndex].value.operand = 0.0;
                    outputIndex++;
                }

                if (isdigit(*myFunction) || *myFunction == '.') {
                    while (isdigit(*myFunction) || *myFunction == '.') {
                        if (*myFunction == '.') {
                            decimal_flag = 1;
                        } else {
                            if (decimal_flag == 0) {
                                operand = operand * 10 + (*myFunction - '0');
                            } else {
                                decimal_divisor *= 10;
                                operand = operand + (*myFunction - '0') / decimal_divisor;
                            }
                        }
                        myFunction++;
                    }
                    if (minus == 1) {
                        operand = -operand;
                        minus = 0; 
                    }
                    output[outputIndex].type = OPERAND;
                    output[outputIndex].value.operand = operand;
                    outputIndex++;
                } else if (*myFunction == 'x') {
                    myFunction++;
                    output[outputIndex].type = VARIABLE;
                    output[outputIndex].value.variable = 0.0;
                    outputIndex++;                    
                }

                if (isOperator(*myFunction)) {
                    current_state = S_OPERATOR;
                } else if (*myFunction == ')') {
                    current_state = S_CLOSE;
                } else if (*myFunction == 0) {
                    current_state = S_END;
                } else {
                    current_state = S_ERROR;
                }
                break;
            }

            case S_OPERATOR:
                while (stackTop >= 0 && isOperator(stack[stackTop]) && 
                       precedence(stack[stackTop]) >= precedence(*myFunction)) {
                    output[outputIndex].type = OPERATOR;
                    output[outputIndex].value.operator = stack[stackTop];
                    outputIndex++;
                    stackTop--;
                }
                stack[++stackTop] = *myFunction;
                myFunction++;
                current_state = S_START;
                break;

            case S_OPEN:
                stack[++stackTop] = *myFunction;
                myFunction++;
                current_state = S_START;
                break;

            case S_CLOSE:
                openParentheses--; // Gi?m bi?n d?m d?u ngo?c
                while (stackTop >= 0 && stack[stackTop] != '(') {
                    output[outputIndex].type = OPERATOR;
                    output[outputIndex].value.operator = stack[stackTop];
                    outputIndex++;
                    stackTop--;
                }
                if (stackTop >= 0) stackTop--; 
                myFunction++;
                if (isOperator(*myFunction)) {
                    current_state = S_OPERATOR;
                } else if (*myFunction == ')') {
                    current_state = S_CLOSE;
                } else if (*myFunction == 0) {
                    current_state = S_END;
                } else if (isdigit(*myFunction) || *myFunction == '.') {
                    current_state = S_OPERAND;
                } else {
                    current_state = S_ERROR;
                }
                break;

            case S_END:
                while (stackTop >= 0) {
                    output[outputIndex].type = OPERATOR;
                    output[outputIndex].value.operator = stack[stackTop];
                    outputIndex++;
                    stackTop--;
                }
                output[outputIndex].type = OPERATOR;
                output[outputIndex].value.operator = 'E';
                outputIndex++;
                if (openParentheses > 0) {
                    SSD1306_Clear();
										SSD1306_GotoXY(0, 0);
										SSD1306_Puts("Loi bieu thuc!", &Font_7x10, 1);
										SSD1306_UpdateScreen();
                    return NULL;
                }
                return output;
                break;

            case S_ERROR:
            default:
                SSD1306_Clear();
                SSD1306_GotoXY(0, 0);
                SSD1306_Puts("Loi bieu thuc!", &Font_7x10, 1);
                SSD1306_UpdateScreen();
                return NULL;
        }
    }
}

float evaluatePostfix(Token *postfix, float x) {
    float stack[MAX];
    int top = -1;
    for (int i = 0; postfix[i].type != OPERATOR || postfix[i].value.operator != 'E'; i++) {
        if (postfix[i].type == OPERAND) {
            stack[++top] = postfix[i].value.operand;
        } else if (postfix[i].type == VARIABLE) {
            stack[++top] = x; 
        } else if (postfix[i].type == OPERATOR) {
            if (postfix[i].value.operator == 'c') { 
                float b = stack[top--];
                stack[++top] = cos(b);
            } else if (postfix[i].value.operator == 's') { 
                float b = stack[top--];
                stack[++top] = sin(b);
            } else {
                float b = stack[top--];
                float a = stack[top--];
                switch (postfix[i].value.operator) {
                    case '+': stack[++top] = a + b; break;
                    case '-': stack[++top] = a - b; break;
                    case '*': stack[++top] = a * b; break;
                    case '/': stack[++top] = a / b; break;
                    case '^': stack[++top] = pow(a, b); break;
                }
            }
        }
    }
    return stack[top]; 
}\
void printTokens(Token *output) {
    int i = 0;
		int j =10;
    char buffer[20];

    SSD1306_Clear();
    SSD1306_GotoXY(0, 0);
    SSD1306_Puts("Postfix:", &Font_7x10, 1);
    SSD1306_GotoXY(0, j);

    while (output[i].type != OPERATOR || output[i].value.operator != 'E') {
        if (output[i].type == OPERAND) {
            sprintf(buffer, "%.2f", output[i].value.operand);
            SSD1306_Puts(buffer, &Font_7x10, 1);
						if (i==21|| i == 41|| i ==51) 
							{j+=10;
								SSD1306_GotoXY(0, j);
								}
        } else if (output[i].type == VARIABLE) {
            SSD1306_Puts("x", &Font_7x10, 1);
					if (i==21|| i == 41|| i ==51)
							{j+=10;
								SSD1306_GotoXY(0, j);
								}
        } else if (output[i].type == OPERATOR) {
            char op[3] = {output[i].value.operator, '\0', '\0'};
            SSD1306_Puts(op, &Font_7x10, 1);
					if (i==21|| i == 41|| i ==51) 
							{j+=10;
								SSD1306_GotoXY(0, j);
								}
        }
        i++;
    }

    SSD1306_UpdateScreen();
}

