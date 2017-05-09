#include "common.h"

long s24(long val) {
	return (val << (sizeof(long)*8 - 24)) >> (sizeof(long)*8 - 24);
}
long s12(long val) {
	return (val << (sizeof(long)*8 - 12)) >> (sizeof(long)*8 - 12);
}
