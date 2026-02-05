

#include "main.h"
#include "ssd1306.h"
#include "fonts.h"
#include "postfix.h" 
#include <stdio.h>

#define NUM_ROWS 5
#define NUM_COLS 4

#define MAX_EXPR_LEN 100
char buffer[MAX_EXPR_LEN];
int bufferIndex = 0;

const uint8_t rowPins[NUM_ROWS] = {GPIO_PIN_7, GPIO_PIN_6, GPIO_PIN_5, GPIO_PIN_4, GPIO_PIN_3}; // PORTA nguoc chieu
const uint8_t colPins[NUM_COLS] = {GPIO_PIN_1, GPIO_PIN_0, GPIO_PIN_3, GPIO_PIN_4};             // PORTB

char keymap[NUM_ROWS][NUM_COLS] = {
    {'(', ')', '/', '*'},
    {'1', '2', '3', '+'},
    {'4', '5', '6', '-'},
    {'7', '8', '9', 'D'},
    {'x', '0', '^', '.'}
};

void printFloat(float value, uint16_t x, uint16_t y) {
    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%.4f", value);  // Format s? th?c v?i 2 ch? s? sau d?u ch?m
    SSD1306_GotoXY(x, y);
    SSD1306_Puts(buffer, &Font_7x10, 1);
    SSD1306_UpdateScreen();
}

void printString(uint16_t x, uint16_t y, char* arr){
			SSD1306_GotoXY(x,y);
			SSD1306_Puts(arr, &Font_7x10, 1);
			SSD1306_UpdateScreen();
	}

char scanKeypad(void) {
    for (int row = 0; row < NUM_ROWS; row++) {
        HAL_GPIO_WritePin(GPIOA, rowPins[row], GPIO_PIN_RESET);
        HAL_Delay(1);
        for (int col = 0; col < NUM_COLS; col++) {
            if (HAL_GPIO_ReadPin(GPIOB, colPins[col]) == GPIO_PIN_RESET) {
                HAL_Delay(20);
                while (HAL_GPIO_ReadPin(GPIOB, colPins[col]) == GPIO_PIN_RESET);
                HAL_GPIO_WritePin(GPIOA, rowPins[row], GPIO_PIN_SET);
                return keymap[row][col];
            }
        }
        HAL_GPIO_WritePin(GPIOA, rowPins[row], GPIO_PIN_SET);
    }
    return 0;
}
// Bi?n toàn c?c d? theo dõi phím tru?c dó
static char lastKey = '\0';

char* printKey(uint16_t *x, uint16_t *y, char key) {
    // X? lý phím xóa (D)
    if (key == 'D') {
        if (bufferIndex > 0) {
            bufferIndex--;
            buffer[bufferIndex] = '\0';
            if (*x >= 6) *x -= 6;
            else {
                if (*y >= 10) *y -= 10;
                *x = 120;
            }
            SSD1306_GotoXY(*x, *y);
            SSD1306_Puts(" ", &Font_7x10, 1);
            SSD1306_UpdateScreen();
        }
        lastKey = '\0'; // Reset lastKey
        return NULL;
    }

    // X? lý t? h?p phím '+.' ho?c '-.'
    if (key == '.' && (lastKey == '+' || lastKey == '-')) {
        // Xóa ký t? tru?c dó (+ ho?c -) kh?i buffer và màn hình
        if (bufferIndex > 0) {
            bufferIndex--;
            buffer[bufferIndex] = '\0';
            if (*x >= 6) *x -= 6;
            SSD1306_GotoXY(*x, *y);
            SSD1306_Puts(" ", &Font_7x10, 1);
        }

        // Thêm "cos" ho?c "sin" tùy theo lastKey
        if (lastKey == '+' && bufferIndex + 3 < MAX_EXPR_LEN - 1) {
            strcpy(&buffer[bufferIndex], "cos");
            bufferIndex += 3;
            buffer[bufferIndex] = '\0';
            SSD1306_GotoXY(*x, *y);
            SSD1306_Puts("cos", &Font_7x10, 1);
            SSD1306_UpdateScreen();
            *x += 18; // 3 ký t? x 6 pixel
        }
        else if (lastKey == '-' && bufferIndex + 3 < MAX_EXPR_LEN - 1) {
            strcpy(&buffer[bufferIndex], "sin");
            bufferIndex += 3;
            buffer[bufferIndex] = '\0';
            SSD1306_GotoXY(*x, *y);
            SSD1306_Puts("sin", &Font_7x10, 1);
            SSD1306_UpdateScreen();
            *x += 18; // 3 ký t? x 6 pixel
        }

        // C?p nh?t t?a d? hi?n th?
        if (*x > 120) {
            *x = 0;
            *y += 10;
            if (*y > 55) {
                SSD1306_Clear();
                *y = 11;
                *x = 0;
            }
        }
        lastKey = key; // C?p nh?t lastKey
        return NULL;
    }

    // X? lý các phím thông thu?ng
    SSD1306_GotoXY(*x, *y);
    SSD1306_Putc(key, &Font_7x10, 1);
    SSD1306_UpdateScreen();

    if (bufferIndex < MAX_EXPR_LEN - 1) {
        buffer[bufferIndex++] = key;
        buffer[bufferIndex] = '\0';
    }

    *x += 6;
    if (*x > 120) {
        *x = 0;
        *y += 10;
        if (*y > 55) {
            SSD1306_Clear();
            *y = 11;
            *x = 0;
        }
    }

    // Ki?m tra l?i hai d?u ch?m liên ti?p
    if (bufferIndex >= 2 && buffer[bufferIndex - 1] == '.' && buffer[bufferIndex - 2] == '.') {
        if (*x >= 12) *x -= 12;
        else {
            if (*y >= 10) *y -= 10;
            *x = 120;
        }
        SSD1306_GotoXY(*x, *y);
        SSD1306_Puts("  ", &Font_7x10, 1);
        SSD1306_UpdateScreen();
        bufferIndex -= 2;
        buffer[bufferIndex] = '\0';
        return buffer;
    }

    lastKey = key; // C?p nh?t phím cu?i cùng
    return NULL;
}