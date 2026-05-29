#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer0.h"
#include "sevenseg.h"

// 中文: 如果 build system 没有定义 F_CPU，这里默认使用 8 MHz。
// English: If the build system has not defined F_CPU, default to 8 MHz here.
#ifndef F_CPU
#define F_CPU 8000000UL
#endif

// 中文: 这些变量定义在 morse.c 中，Timer0 ISR 用它们同步 seven-seg 状态。
// English: These variables are defined in morse.c; the Timer0 ISR uses them to sync seven-seg state.
extern uint8_t marks_in_current_char;
extern uint8_t submitted_character_count;

// 中文: Timer0 使用 64 分频，计算 1ms compare match 所需的 OCR0A 值。
// English: Timer0 uses a prescaler of 64; this calculates OCR0A for a 1 ms compare match.
#define TIMER0_PRESCALER 64UL
#define TIMER0_1MS_OCR ((F_CPU / TIMER0_PRESCALER / 1000UL) - 1)

// 中文: 计数 1ms tick，累计到 100ms 后产生 animation tick。
// English: Counts 1 ms ticks and produces an animation tick every 100 ms.
static volatile uint8_t ms_counter = 0;

// 中文: 待主循环处理的 animation tick 数量。
// English: Number of pending animation ticks waiting for the main loop to consume.
static volatile uint8_t animation_ticks_pending = 0;

// 中文: 待主循环处理的 1ms tick 数量。
// English: Number of pending 1 ms ticks waiting for the main loop to consume.
static volatile uint8_t ms_ticks_pending = 0;

void timer0_init(void)
{
    // 中文: 先清空 Timer0 控制寄存器，避免旧配置影响新配置。
    // English: Clear Timer0 control registers first so old settings do not affect the new setup.
    TCCR0A = 0;
    TCCR0B = 0;

    // 中文: CTC mode，计数器到 OCR0A 后自动清零。
    // English: CTC mode: clear the timer on compare match with OCR0A.
    TCCR0A |= (1 << WGM01);

    // 中文: 设置约 1ms 的 compare match 值。
    // English: Compare value for approximately 1 ms.
    OCR0A = (uint8_t)TIMER0_1MS_OCR;

    // 中文: 打开 Timer0 compare match A interrupt。
    // English: Enable Timer0 compare match A interrupt.
    TIMSK0 |= (1 << OCIE0A);

    // 中文: 使用 64 分频启动 Timer0。
    // English: Start Timer0 with a prescaler of 64.
    TCCR0B |= (1 << CS01) | (1 << CS00);
}

ISR(TIMER0_COMPA_vect){
    // 中文: 每次 ISR 代表约 1ms，累积一个待处理 1ms tick。
    // English: Each ISR represents about 1 ms, so add one pending 1 ms tick.
    if (ms_ticks_pending < 255) {
        ms_ticks_pending++;

    }

    // 中文: 累计 100 个 1ms tick，产生一个 100ms animation tick。
    // English: Accumulate 100 one-millisecond ticks to produce one 100 ms animation tick.
    ms_counter++;
    if (ms_counter >= 100) {
        ms_counter = 0;


        if (animation_ticks_pending < 255) {
            animation_ticks_pending++;
        }
    }
}

uint8_t timer0_consume_animation_tick(void)
{
    uint8_t result = 0;
    uint8_t old_sreg = SREG;

    // 中文: 读写 pending 计数时临时关中断，避免 ISR 同时修改变量。
    // English: Temporarily disable interrupts while touching the pending counter.
    cli();

    if (animation_ticks_pending > 0) {
        animation_ticks_pending--;
        result = 1;
    }

    SREG = old_sreg;

    return result;
}

void timer0_reset_animation_counter(void)
{
    uint8_t old_sreg = SREG;

    // 中文: 重置计时相关变量时关中断，确保变量状态一致。
    // English: Disable interrupts while resetting timer state so the variables stay consistent.
    cli();
    ms_counter = 0;
    animation_ticks_pending = 0;
    ms_ticks_pending = 0;
    SREG = old_sreg;
}

uint8_t timer0_consume_1ms(void) {
    uint8_t result = 0;
    uint8_t old_sreg = SREG;// 中文: 保存全局中断状态。 / English: Save the global interrupt state.
    cli();// 中文: 临时关闭全局中断。 / English: Temporarily disable global interrupts.
    if (ms_ticks_pending > 0){
        ms_ticks_pending--; // 中文: 消耗一个待处理 1ms tick。 / English: Consume one pending 1 ms tick.
        result = 1;
    }
    // 中文: 恢复进入函数前的中断状态。
    // English: Restore the interrupt state from before this function was entered.
    SREG = old_sreg; 
    return result;
}
