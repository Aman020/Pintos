#ifndef FIX_H
#define FIX_H

#include <stdio.h>

int32_t f = 1 << 14;

int ftoint(int32_t x);

int32_t inttof(int x);

int32_t divide(int32_t x, int32_t y);

int32_t multiply(int32_t x, int32_t y);

int32_t add(int32_t x, int32_t y);

int32_t sub(int32_t x, int32_t y);


int32_t dividen(int32_t x, int n);

int32_t multiplyn(int32_t x, int n);

int32_t addn(int32_t x, int n);

int32_t subn(int32_t x, int n);

#endif
