#include "postfix.h"

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

Token *infixToPostfix(char* myFunction){
    state_t current_state = S_START;
    Token *output = (Token *)malloc(MAX * sizeof(Token));
    int outputIndex = 0;
    char stack[MAX];
    int stackTop = -1;
    int openParentheses = 0; // Biến đếm dấu mở ngoặc

    while (1){
        switch (current_state){
            case S_START:
                if (isdigit(*myFunction) || *myFunction == '.' || *myFunction == 'x') {
                    current_state = S_OPERAND;
                } else if (*myFunction == '(') {
		            openParentheses++; // Tăng biến đếm
                    current_state = S_OPEN;
                } else if (*myFunction == 0) {
                    current_state = S_END;
                } else if (strncmp(myFunction, "cos", 3) == 0) {
                    stack[++stackTop] = 'c'; // Đánh dấu hàm cos
                    myFunction += 3; // Bỏ qua "cos"
                    if (*myFunction == '(') {
                        current_state = S_START;
                    } else {
                        current_state = S_ERROR; // Nếu không có dấu mở ngoặc, báo lỗi
                    }
                } else if (strncmp(myFunction, "sin", 3) == 0) {
                    stack[++stackTop] = 's'; // Đánh dấu hàm sin
                    myFunction += 3; // Bỏ qua "sin"
                    if (*myFunction == '(') {
                        current_state = S_START;
                    } else {
                        current_state = S_ERROR; // Nếu không có dấu mở ngoặc, báo lỗi
                    }
                } else {
                    current_state = S_ERROR; // Nếu là ký tự không hợp lệ
                 }
                break;
            case S_OPERAND: {
                float operand = 0.0;
                int decimal_flag = 0;
                float decimal_divisor = 1.0;
                if (isdigit(*myFunction) || *myFunction == '.'){
                    while (isdigit(*myFunction) || *myFunction == '.') {
                        if (*myFunction == '.') {
                            decimal_flag = 1;
                        }
                        else {
                            if (decimal_flag == 0) {
                                operand = operand * 10 + (*myFunction - '0');
                            }
                            else {
                                decimal_divisor *= 10;
                                operand = operand + (*myFunction - '0') / decimal_divisor;
                            }
                        }
                        myFunction++;
                    }
                    output[outputIndex].type = OPERAND;
                    output[outputIndex].value.operand = operand;
                    outputIndex++;
                }
                else if (*myFunction == 'x') {
                    myFunction++;
                    output[outputIndex].type = VARIABLE;
                    output[outputIndex].value.variable = 0.0;
                    outputIndex++;                    
                }       
                if (isOperator(*myFunction)) {
                    current_state = S_OPERATOR;
                }
                else if (*myFunction == ')') {
                    current_state = S_CLOSE;
                }
                else if (*myFunction == 0) {
                    current_state = S_END;
                }
                else {
                    current_state = S_ERROR;
                }
                break;
            }

            case S_OPERATOR:
                while (stackTop >= 0 && isOperator(stack[stackTop]) && 
                       precedence(stack[stackTop]) >= precedence(*myFunction)) {
                    output[outputIndex].type = OPERATOR;
                    output[outputIndex].value.op = stack[stackTop];
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
                openParentheses--;
                while (stackTop >= 0 && stack[stackTop] != '(') {
                    output[outputIndex].type = OPERATOR;
                    output[outputIndex].value.op = stack[stackTop];
                    outputIndex++;
                    stackTop--;
                    
                }
                if (stackTop >= 0) stackTop--; 
                myFunction++;
                if (isOperator(*myFunction)) {
                    current_state = S_OPERATOR;
                }
                else if (*myFunction == ')') {
                    current_state = S_CLOSE;
                }
                else if (*myFunction == 0) {
                    current_state = S_END;
                }
                else if (isdigit(*myFunction) || *myFunction == '.') {
                    current_state = S_OPERAND;
                }
                else {
                    current_state = S_ERROR;
                }
                break;

            case S_END:
                while (stackTop >= 0) {
                    output[outputIndex].type = OPERATOR;
                    output[outputIndex].value.op = stack[stackTop];
                    outputIndex++;
                    stackTop--;
                }
                output[outputIndex].type = OPERATOR;
                output[outputIndex].value.op = 'E';
                outputIndex++;

		        if (openParentheses > 0) {
                    printf("Lỗi cú pháp, hãy bắt đầu lại.\n");
                    return NULL;
                }

                return output;


            case S_ERROR:
            default:
                printf("Input function error!!!\n");
                return NULL;
        }
    }
}
float evaluatePostfix(Token *postfix, float x) {
    float stack[MAX];
    int top = -1;
    for (int i = 0; postfix[i].type != OPERATOR || postfix[i].value.op != 'E'; i++) {
        if (postfix[i].type == OPERAND) {
            stack[++top] = postfix[i].value.operand;
        } else if (postfix[i].type == VARIABLE) {
            stack[++top] = x; 
        } else if (postfix[i].type == OPERATOR) {
            if (postfix[i].value.op == 'c') { // Hàm cos
                float b = stack[top--];
                stack[++top] = cos(b);
            } else if (postfix[i].value.op == 's') { // Hàm sin
                float b = stack[top--];
                stack[++top] = sin(b);
            } else {
                float b = stack[top--];
                float a = stack[top--];
                switch (postfix[i].value.op) {
                    case '+': stack[++top] = a + b; break;
                    case '-': stack[++top] = a - b; break;
                    case '*': stack[++top] = a * b; break;
                    case '/':
                        if (b != 0) {
                            stack[++top] = a / b;
                        } else {
                            printf("Lỗi: Chia cho 0!\n");
                            return NAN; // Trả về NaN nếu chia cho 0
                        }
                        break;
                    case '^': stack[++top] = pow(a, b); break;
                }
            }
        }
    }
    return stack[top]; 
}
void printTokens(Token *output) {
    int i = 0;
    printf("Output Tokens: ");
    while (output[i].type != OPERATOR || output[i].value.op != 'E') {
        if (output[i].type == OPERAND) {
            printf("%.2f ", output[i].value.operand);
        } else if (output[i].type == OPERATOR) {
            printf("%c ", output[i].value.op);
        } else if (output[i].type == VARIABLE) {
            printf("x ");
        }
        i++;
    }
    printf("\n");
}
