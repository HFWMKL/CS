#include <stdint.h>
/*
 * joystick.c
 *
 * 中文: Joystick 功能的实现文件，目前还没有实际代码。
 * English: Implementation file for joystick functionality; currently no executable code is present.
 *
 * 中文: 以后可以在这里实现 ADC 初始化、读取 x/y 轴电压、根据倾斜角度计算滚动速度等逻辑。
 * English: Future logic can include ADC setup, x/y axis reading, and mapping joystick tilt to scroll speed.
 *
 * 中文: 本次只添加注释，不添加任何会影响编译或运行行为的代码。
 * English: This pass only adds comments and does not add code that changes compilation or runtime behaviour.
 */

#define HISTORY_CHAR 50;
uint8_t column; 
uint16_t char_history[HISTORY_CHAR];
uint8_t pass_letter_start = 0;// control the whether pass the oldest sign
uint16_t submitted_char = 0;
//要杆控制->刷新屏幕-

static move_stick(char_history, column) {
    if (HISTORY_CHAR > submitted_char) {
        
    }

    else {
    
    }


}
static void add_history_char(character) {
    pass_letter_start = (pass_letter_start + 1) % HISTORY_CHAR
    char_history[pass_letter_start] = character

}

static void refresh screen() {

    if (char_history > 50) {
        passflag = 1
    }

    if ()


}