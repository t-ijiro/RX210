#ifndef TIMER_EVENT_T_H
#define TIMER_EVENT_T_H

typedef enum {
    TASK_NONE               = 0,
    TASK_DYNAMIC            = 1 << 0,
    TASK_GRADATION          = 1 << 1,
    TASK_SCROLL             = 1 << 2
} timer_event_t;

#endif /* TIMER_EVENT_T_H */
