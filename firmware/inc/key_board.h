#ifndef __KEY_BOARD_H
#define __KEY_BOARD_H
#include <stdint.h>

char scanKeypad(void);
void printFloat(float value, uint16_t x, uint16_t y);
void printString(uint16_t x, uint16_t y, char* arr);
char* printKey(uint16_t *x, uint16_t *y, char key);
#endif
