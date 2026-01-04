#ifndef TIMER_EVENT_T_H
#define TIMER_EVENT_T_H

typedef enum {
    CLR_ALL_FLAG            = 0,
    TASK_GEN_SOFTWARE_TIMER = 1 << 0,
    TASK_DYNAMIC            = 1 << 1,
    TASK_GRADATION          = 1 << 2,
    TASK_SCROLL             = 1 << 3
} timer_event_t;

#endif /* TIMER_EVENT_T_H */