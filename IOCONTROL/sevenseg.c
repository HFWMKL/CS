

#include "sevenseg.h"
#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>

// 中文: 段选端口。PA0-PA7 通常连接 seven-seg 的 A-G 和 DP 段线。
// English: Segment-select port. PA0-PA7 usually connect to the A-G and DP segment lines.
#define SEG_DDR   DDRA
#define SEG_PORT  PORTA

// 中文: 位选端口。这里用 PD0/PD1 控制左/右 digit 是否被选中。
// English: Digit-select port. PD0/PD1 are used here to select the left/right digit.
#define DIGIT_DDR   DDRD
#define DIGIT_PORT  PORTD

// 中文: 左、右 digit 的位选 bit mask。
// English: Bit masks for selecting the left and right digits.
#define LEFT_DIGIT_MASK   (1 << PD4)
#define RIGHT_DIGIT_MASK  (1 << PD5)

// 中文: 小数点段的 bit mask，对应 PA7/DP。
// English: Bit mask for the decimal point segment, corresponding to PA7/DP.
#define DP_MASK 0x80  //1000 0000

// 中文: 只点亮中间横段，用来显示 '-'。
// English: Lights only the middle segment to display '-'.
#define DASH_PATTERN 0x40 //0100 0000

// 中文: 左 digit 当前应该显示的段码。
// English: Current segment pattern to display on the left digit.
static uint8_t l_number = 0;

// 中文: 右 digit 当前应该显示的段码。
// English: Current segment pattern to display on the right digit.
static uint8_t r_number = 0;

// 中文: 右 digit 是否应该打开。0 表示右 digit 关闭。
// English: Whether the right digit should be enabled. 0 means the right digit is off.
static uint8_t r_open = 0;

// 中文: 十六进制 0-F 的 seven-seg 段码表。
// English: Seven-segment patterns for hexadecimal digits 0-F.
static const uint8_t sevenseg_patterns[16] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F, // 9
    0x77, // A
    0x7C, // b
    0x39, // C
    0x5E, // d
    0x79, // E
    0x71  // F
};

void sevenseg_init(void)
{
    // 中文: PA0-PA7 全部设置为输出，用来控制 A-G/DP 段线。
    // English: Set all PA0-PA7 pins as outputs for the A-G/DP segment lines.
    SEG_DDR = 0xFF;

    // 中文: PD0/PD1 设置为输出，用来控制左右 digit 位选。
    // English: Set PD0/PD1 as outputs for the left/right digit-select lines.
    DIGIT_DDR |= LEFT_DIGIT_MASK | RIGHT_DIGIT_MASK;

    // 中文: 初始左边准备显示 0，右边关闭。
    // English: Initially prepare the left digit to show 0 and keep the right digit off.
    l_number = sevenseg_patterns[0];
    r_number = 0;
    r_open = 0;

    // 中文: 段线清零，并关闭位选输出。
    // English: Clear segment lines and disable digit-select outputs.
    SEG_PORT = 0x00;
    DIGIT_PORT &= ~(LEFT_DIGIT_MASK | RIGHT_DIGIT_MASK);
}
//subChCount submitted character count已经提交的字符数量， marks_in_current_char current input 当前正在输入的 Morse 字符已经输入了几个 mark
void sevenseg_update_state(uint8_t subChCount, uint8_t marks_in_current_char) {
    // 中文: 左 digit 显示已提交字符数量，取低 4 bit 实现 modulo 16。
    // English: Left digit shows submitted character count; low 4 bits implement modulo 16.
    l_number = sevenseg_patterns[subChCount % 16];

    // 中文: 没有正在输入的字符时，右 digit 关闭。
    // English: If there is no in-progress character, turn off the right digit.
    if (marks_in_current_char == 0) {
        r_number = 0;
        r_open = 0;
    } else {
        // 中文: 有正在输入的字符时，右 digit 显示 mark 数量。
        // English: If a character is in progress, right digit shows the number of marks.
        r_open = 1;
        if (marks_in_current_char > 9) {
            // 中文: mark 数量超过 9 时，按作业要求显示 '-'。
            // English: If the mark count is above 9, display '-' as required.
            r_number = DASH_PATTERN;
        } else {
            r_number = sevenseg_patterns[marks_in_current_char];
        }
    }
}

void sevenseg_refresh(void){
    // 中文: l_open 每次刷新翻转一次，用来在左/右 digit 之间快速轮流显示。
    // English: l_open toggles each refresh to multiplex between left and right digits.
    static uint8_t l_open = 0;

    // 中文: 先关闭两个 digit，避免切换段码时产生 ghosting。
    // English: Disable both digits first to reduce ghosting while changing segment data.
    DIGIT_PORT &= ~(LEFT_DIGIT_MASK | RIGHT_DIGIT_MASK);
    
    if (l_open) {
        // 中文: 准备左 digit 的段码。
        // English: Prepare the segment pattern for the left digit.
        uint8_t pattern = l_number;
        if (r_open) {
            
            pattern |= DP_MASK;   // 中文: 右边开着时，左边小数点亮。 / English: Turn on left decimal point iff right digit is on.
        }
        SEG_PORT = pattern;
        DIGIT_PORT |= LEFT_DIGIT_MASK;
    }else{
        if (r_open) {
            // 中文: 只有右 digit 需要显示时，才输出右边段码并打开右 digit。
            // English: Only output and enable the right digit when it should be visible.
            SEG_PORT = r_number;
            DIGIT_PORT |= RIGHT_DIGIT_MASK;
        }
        else{
            // 中文: 右 digit 关闭时，不显示任何段码。
            // English: If the right digit is off, output no segment pattern.
            SEG_PORT = 0x00;
        }
    }

    // 中文: 下次 refresh 切换到另一边 digit。
    // English: Switch to the other digit on the next refresh.
    l_open = ! l_open;
};
