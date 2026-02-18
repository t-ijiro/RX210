/*
 * sw.h
 * e2417105
 */

#ifndef SW_H_
#define SW_H_

#include <stdint.h>

#define SW_ON  0
#define SW_OFF 1

typedef struct{
	uint64_t debounce_t;
	uint8_t cur;
	uint8_t pre;
	uint8_t stable;
}sw_t;

void sw_init(void);
uint8_t sw_read(uint8_t sw);

#endif /* SW_H_ */

