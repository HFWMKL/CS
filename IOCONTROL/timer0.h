#ifndef TIMER0_H_
#define TIMER0_H_

#include <stdint.h>

// 中文: 初始化 Timer0，让它产生约 1ms 的 compare match interrupt。
// English: Initialise Timer0 so it produces an approximately 1 ms compare match interrupt.
void timer0_init(void);

// 中文: 主循环调用它来检查是否有 100ms animation tick 等待处理。
// English: The main loop calls this to check whether a 100 ms animation tick is pending.
uint8_t timer0_consume_animation_tick(void);

// 中文: 重置 Timer0 软件计数器和所有待处理 tick。
// English: Reset the Timer0 software counters and all pending ticks.
void timer0_reset_animation_counter(void);

// 中文: 主循环调用它来检查是否有 1ms tick 等待处理。
// English: The main loop calls this to check whether a 1 ms tick is pending.
uint8_t timer0_consume_1ms(void);

#endif
