#pragma once

extern volatile uint32_t tick_count;
extern volatile uint32_t second_count;

#define gettc()		tick_count
#define rtime()		second_count

void counter_init(void);
