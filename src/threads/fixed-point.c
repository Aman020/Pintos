#include <fixed-point.h>

int ftoint(int32_t x) {
	if( x > 0 )
		x += f / 2;
	else
		x -= f / 2;
	return x / f;
}

int32_t inttof(int n) {
	return n * f;
}

int32_t divide(int32_t x, int32_t y) {
	return ((int64_t) x) * f / y;
}

int32_t multiply(int32_t x, int32_t y) {
	return ((int64_t) x) * y / f;
}

int32_t add(int32_t x, int32_t y) {
	return x + y;
}

int32_t sub(int32_t x, int32_t y) {
	return x - y;
}

int32_t dividen(int32_t x, int n) {
	return x / n;
}

int32_t multiplyn(int32_t x, int n) {
	return x * n;
}

int addn(int32_t x, int n) {
	return x + n * f;
}

int subn(int32_t x, int n) {
	return x - n * f;
}
