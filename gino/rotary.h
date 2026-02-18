#ifndef ROTARY_H
#define ROTARY_H

#include <stdint.h>

/* 
 * e2417105
 */

typedef struct{
    int16_t count_new;
    int16_t count_old;
}rotary_t;

typedef enum{
    CLICK_IDLE,
    CLICK_LEFT,
    CLICK_RIGHT
}rotary_click_t;

void rotary_init(void);

void rotary_clear(rotary_t *r);

rotary_t rotary_get_instance(int16_t val_new, int16_t val_old);

void rotary_record_new(rotary_t *r);

void rotary_record_old(rotary_t *r);

int16_t rotary_calc_delta(rotary_t *r);

rotary_click_t rotary_get_click_dir(int16_t delta);

#endif /* ROTARY_H */
