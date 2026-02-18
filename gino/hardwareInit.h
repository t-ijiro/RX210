/* 
 * e2417105
 */

#ifndef HARDWAREINIT_H
#define HARDWAREINIT_H
#include <stdint.h>

void init_CLK(void);
void init_CMT0(uint16_t period_ms, uint8_t is_start);
void init_CMT1(uint16_t period_ms, uint8_t is_start);
void init_CMT2(uint16_t period_ms, uint8_t is_start);
void init_CMT3(uint16_t period_ms, uint8_t is_start);
void init_IRQ0(void);
void init_IRQ1(void);

#endif // HARDWAREINIT_H

