/*
 * display.h
 *
 * Author: Ryan Wang
 *
 * 中文: LED matrix 显示模块的接口声明。
 * English: Public interface for the LED matrix display module.
 */ 
#include <stdint.h>
#include "ledmatrix.h"



#ifndef DISPLAY_H_
#define DISPLAY_H_

// 中文: 小字体每个字符占 3 列。
// English: Each small-font character is 3 columns wide.
#define SMALL_CHAR_WIDTH 3

// 中文: 小字符最右侧显示位置的 x 坐标，也就是 16 列屏幕减去字符宽度。
// English: Rightmost x position for a small character: full screen width minus glyph width.
#define SMALL_CHAR_RIGHT_X (MATRIX_NUM_COLUMNS - SMALL_CHAR_WIDTH)

// 中文: 当前最多保存/显示的已提交字符数量。
// English: Current maximum number of submitted characters stored/displayed.
#define MAX_DISPLAY_CHARS 4

// 中文: 相邻字符之间移动/排列的列间距。小字体 3 列 + 1 空列 = 4。
// English: Column step between characters. Small glyph width 3 + one blank column = 4.
#define CHAR_DISPLAY_STEP 4


/*
 * 中文: 显示启动画面。
 * English: Display the startup splash screen.
 */
void start_splash_display(void);

/*
 * 中文: 从 x_position 开始，在 LED matrix 上画一个小字体字符。
 * English: Draw one small-font character on the LED matrix starting at x_position.
 */
void draw_small_char(char character, uint8_t x_position, uint8_t colour);
// 中文: 显示当前尚未提交的 Morse 字符预览。
// English: Show a preview for the in-progress Morse character.
void display_preview(uint8_t marks_in_current_char, uint8_t current_morse);

// 中文: 添加一个已提交字符，并触发 LED matrix 的字符移动动画。
// English: Add one committed character and start the LED matrix slide animation.
void add_character(char character);

// 中文: 根据当前保存的字符、预览字符和动画偏移，重新绘制整个 LED matrix。
// English: Redraw the full LED matrix from stored characters, preview state, and animation offset.
void redraw_displayed_characters(void);

// 中文: 每次动画 tick 调用一次，让 matrix 动画前进一步。
// English: Advance the LED matrix animation by one step on each animation tick.
void process_next_matrix_animation_step(void);

// 中文: 标记 matrix 动画开始，并把移动距离重置为 0。
// English: Mark matrix animation as active and reset the movement offset to 0.
void start_animation(void);

void add_character_with_colour(char character, uint8_t colour);

#endif 
