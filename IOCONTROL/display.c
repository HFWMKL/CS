/*
 * display.c
 *
 * Author: Ryan Wang
 *
 * 中文: 负责 LED matrix 上的启动画面、字符字体、预览字符和动画显示。
 * English: Handles the LED matrix splash screen, font glyphs, preview character, and display animation.
 */ 

#include <stdint.h>
#include "display.h"
#include "ledmatrix.h"
#include "encoding.h"

// 中文: 保存已经提交并需要显示在 LED matrix 上的字符。
// English: Stores committed characters that should be shown on the LED matrix.
static char displayed_chars[MAX_DISPLAY_CHARS];
static uint8_t displayed_colours[MAX_DISPLAY_CHARS];

// 中文: 当前已经保存的有效字符数量。
// English: Number of valid committed characters currently stored.
static uint8_t displayed_char_count = 0;


// 中文: 标记 LED matrix 字符滑动动画是否正在进行。
// English: Indicates whether the LED matrix slide animation is currently active.
static uint8_t screen_Active = 0; // 是否屏幕有动画 / Whether the screen animation is active.

// 中文: 当前动画已经向左移动了多少列。
// English: Number of columns the current animation has shifted left.
static uint8_t char_Move = 0; // 当前向左移动几列 / Current left-shift amount in columns.

// 中文: 当前未提交字符已经输入了多少个 mark；0 表示没有预览字符。
// English: Number of marks in the in-progress character; 0 means no preview character.
static uint8_t preview_marks = 0;

// 中文: 当前未提交字符的 Morse bit pattern，初始 1 是 prefix。
// English: Morse bit pattern for the in-progress character; initial 1 is the prefix bit.
static uint8_t preview_morse = 1;

/**
 * 中文: 取得小字体字符的某一列 bitmap。
 * English: Gets a specified column bitmap of a small-font glyph.
 */
uint8_t get_small_glyph_column(char c, uint8_t col);

/**
 * 中文: 把 ASCII 字符映射到 font_small/font_large 的数组下标。
 * English: Maps an ASCII character to its corresponding font array index.
 */
uint8_t char_to_glyph_index(char c);


// 中文: 启动画面的 16 列 bitmap，每个 uint8_t 表示一列的 8 个像素。
// English: 16-column bitmap for the splash screen; each uint8_t represents one column of 8 pixels.
static const uint8_t splash_display[MATRIX_NUM_COLUMNS] =
    { 
        0b00000000,  // far left column
        0b00011011, 
        0b00011011, 
        0b11000000, 
        0b11011000, 
        0b00011011, 
        0b00000011, 
        0b11011011, 

        0b11011011, 
        0b11000011, 
        0b11011011, 
        0b11011000, 
        0b11011000, 
        0b00011011, 
        0b00011011, 
        0b00000000   // far right column
    };

void start_splash_display(void)
{
    // 中文: 这个数组保存当前列中每一行应该显示的颜色。
    // English: This array stores the colour for each row in the current column.
    uint8_t column_colour_data[MATRIX_NUM_ROWS];

    // 中文: col_data 是当前列的 8-bit bitmap，会逐 bit 右移处理。
    // English: col_data is the 8-bit bitmap for the current column and is shifted bit by bit.
    uint8_t col_data;

    ledmatrix_clear(); // 中文: 先清空 LED matrix。 / English: Clear the LED matrix first.
    for (uint8_t col = 0; col < MATRIX_NUM_COLUMNS; col++)
    {
        col_data = splash_display[col];
        // 中文: 遍历这一列的 8 个 bit，把 1 映射成绿色，把 0 映射成黑色。
        // English: Walk through the 8 bits in this column: 1 becomes green, 0 becomes black.
        for(uint8_t i = 0; i < MATRIX_NUM_ROWS; i++)
        {
            if(col_data & 0x01)
            {
                column_colour_data[i] = COLOUR_GREEN;
            }
            else
            {
                column_colour_data[i] = COLOUR_BLACK;
            }
            col_data >>= 1;
        }
        ledmatrix_update_column(col, column_colour_data);
    }
}

// 中文: 小字体是 6 行 x 3 列，并通过空白 bit 居中到 8 行。
// English: Small font is 6 rows by 3 columns, padded/centered within 8 rows.
// 中文: 每个字符的第一个 uint8_t 是最左边一列。
// English: The first uint8_t in each glyph is the leftmost column.
// 中文: 每个 uint8_t 的最高位表示顶部像素。
// English: The MSB of each uint8_t represents the top pixel.
const uint8_t font_small[38][3] =
    {
        // Letters A - Z
        { 0b00111110, 0b01001000, 0b00111110 }, // 'A'
        { 0b01111110, 0b01010010, 0b00101100 }, // 'B'
        { 0b00111100, 0b01000010, 0b01000010 }, // 'C'
        { 0b01111110, 0b01000010, 0b00111100 }, // 'D'
        { 0b01111110, 0b01010010, 0b01000010 }, // 'E'
        { 0b01111110, 0b01010000, 0b01000000 }, // 'F'
        { 0b00111100, 0b01001010, 0b01001110 }, // 'G'
        { 0b01111110, 0b00010000, 0b01111110 }, // 'H'
        { 0b01000010, 0b01111110, 0b01000010 }, // 'I'
        { 0b00001100, 0b00000010, 0b01111100 }, // 'J'
        { 0b01111110, 0b00010000, 0b01101110 }, // 'K'
        { 0b01111110, 0b00000010, 0b00000010 }, // 'L'
        { 0b01111110, 0b00110000, 0b01111110 }, // 'M'
        { 0b01111110, 0b01000000, 0b01111110 }, // 'N'
        { 0b00111100, 0b01000010, 0b00111100 }, // 'O'
        { 0b01111110, 0b01001000, 0b00110000 }, // 'P'
        { 0b00111100, 0b01000110, 0b00111110 }, // 'Q'
        { 0b01111110, 0b01001000, 0b00110110 }, // 'R'
        { 0b00100010, 0b01010010, 0b01001100 }, // 'S'
        { 0b01000000, 0b01111110, 0b01000000 }, // 'T'
        { 0b01111100, 0b00000010, 0b01111110 }, // 'U'
        { 0b01111100, 0b00000010, 0b01111100 }, // 'V'
        { 0b01111110, 0b00001100, 0b01111110 }, // 'W'
        { 0b01100110, 0b00011000, 0b01100110 }, // 'X'
        { 0b01100000, 0b00011110, 0b01100000 }, // 'Y'
        { 0b01000110, 0b01011010, 0b01100010 }, // 'Z'
        // Numbers 0 - 9
        { 0b00111100, 0b01010010, 0b00111100 }, // '0'
        { 0b00100010, 0b01111110, 0b00000010 }, // '1'
        { 0b01001110, 0b01010010, 0b00100010 }, // '2'
        { 0b01000010, 0b01010010, 0b00111100 }, // '3'
        { 0b01110000, 0b00010000, 0b01111110 }, // '4'
        { 0b01110010, 0b01010010, 0b01001100 }, // '5'
        { 0b00111100, 0b01010010, 0b01001100 }, // '6'
        { 0b01000000, 0b01011110, 0b01100000 }, // '7'
        { 0b00101100, 0b01010010, 0b00101100 }, // '8'
        { 0b00110010, 0b01001010, 0b00111100 }, // '9'
        
        { 0b00000000, 0b00000000, 0b00000000 },  // ' '
        { 0b00100000, 0b01001010, 0b00110000 }  // '?'
    };

// 中文: 大字体是 8 行 x 5 列，后续 Font Selection 功能可以用它。
// English: Large font is 8 rows by 5 columns and can be used by the Font Selection feature.
const uint8_t font_large[38][5] =
    {
        // Letters A - Z
        { 0b01111111, 0b10001000, 0b10001000, 0b10001000, 0b01111111 }, // 'A'
        { 0b11111111, 0b10010001, 0b10010001, 0b10010001, 0b01101110 }, // 'B'
        { 0b01111110, 0b10000001, 0b10000001, 0b10000001, 0b01000010 }, // 'C'
        { 0b11111111, 0b10000001, 0b10000001, 0b10000001, 0b01111110 }, // 'D'
        { 0b11111111, 0b10010001, 0b10010001, 0b10010001, 0b10000001 }, // 'E'
        { 0b11111111, 0b10010000, 0b10010000, 0b10010000, 0b10000000 }, // 'F'
        { 0b01111110, 0b10000001, 0b10001001, 0b10001001, 0b01000110 }, // 'G'
        { 0b11111111, 0b00010000, 0b00010000, 0b00010000, 0b11111111 }, // 'H'
        { 0b10000001, 0b10000001, 0b11111111, 0b10000001, 0b10000001 }, // 'I'
        { 0b00000110, 0b00000001, 0b00000001, 0b00000001, 0b11111110 }, // 'J'
        { 0b11111111, 0b00011000, 0b00100100, 0b01000010, 0b10000001 }, // 'K'
        { 0b11111111, 0b00000001, 0b00000001, 0b00000001, 0b00000001 }, // 'L'
        { 0b11111111, 0b01000000, 0b00100000, 0b01000000, 0b11111111 }, // 'M'
        { 0b11111111, 0b01000000, 0b00110000, 0b00001000, 0b11111111 }, // 'N'
        { 0b01111110, 0b10000001, 0b10000001, 0b10000001, 0b01111110 }, // 'O'
        { 0b11111111, 0b10010000, 0b10010000, 0b10010000, 0b01100000 }, // 'P'
        { 0b01111110, 0b10000001, 0b10000101, 0b10000011, 0b01111111 }, // 'Q'
        { 0b11111111, 0b10011000, 0b10010100, 0b10010010, 0b01100001 }, // 'R'
        { 0b01100010, 0b10010001, 0b10010001, 0b10010001, 0b01001110 }, // 'S'
        { 0b10000000, 0b10000000, 0b11111111, 0b10000000, 0b10000000 }, // 'T'
        { 0b11111110, 0b00000001, 0b00000001, 0b00000001, 0b11111110 }, // 'U'
        { 0b11111000, 0b00000110, 0b00000001, 0b00000110, 0b11111000 }, // 'V'
        { 0b11111111, 0b00000010, 0b00001100, 0b00000010, 0b11111111 }, // 'W'
        { 0b11000011, 0b00100100, 0b00011000, 0b00100100, 0b11000011 }, // 'X'
        { 0b11000000, 0b00100000, 0b00011111, 0b00100000, 0b11000000 }, // 'Y'
        { 0b10000011, 0b10000101, 0b10001001, 0b10010001, 0b11100001 }, // 'Z'
        
        // Numbers 0 - 9
    	{ 0b01111110, 0b10000101, 0b10001001, 0b10010001, 0b01111110 }, // '0'
        { 0b00100001, 0b01000001, 0b11111111, 0b00000001, 0b00000001 }, // '1'
        { 0b01000011, 0b10000101, 0b10001001, 0b10010001, 0b01100001 }, // '2'
        { 0b01000010, 0b10000001, 0b10001001, 0b10001001, 0b01110110 }, // '3'
        { 0b00011000, 0b00101000, 0b01001000, 0b11111111, 0b00001000 }, // '4'
        { 0b11110010, 0b10010001, 0b10010001, 0b10010001, 0b10001110 }, // '5'
        { 0b01111110, 0b10010001, 0b10010001, 0b10010001, 0b01001110 }, // '6'
        { 0b10000000, 0b10000000, 0b10001111, 0b10010000, 0b11100000 }, // '7'
        { 0b01101110, 0b10010001, 0b10010001, 0b10010001, 0b01101110 }, // '8'
        { 0b01110010, 0b10001001, 0b10001001, 0b10001001, 0b01111110 }, // '9'
        
        // Symbol
        { 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000 },  // ' '
        { 0b01000000, 0b10000000, 0b10001011, 0b10010000, 0b01100000 }  // '?'
    };

void draw_small_char(char character, uint8_t x_position, uint8_t colour)
{
    // 中文: 这个函数把字符的 3 列 glyph bitmap 转换成 LED matrix 所需的颜色列。
    // English: This function converts the 3 glyph bitmap columns into LED matrix colour columns.
	// 中文: 1 个 bit 表示该像素点亮，0 表示该像素熄灭。
	// English: A 1 bit means the pixel is lit; a 0 bit means the pixel is off.

    // 中文: LED matrix 底层接口需要一整列 8 行的颜色数据。
    // English: The low-level LED matrix API needs colour data for all 8 rows of one column.
    uint8_t column_colour_data[MATRIX_NUM_ROWS];

    for (uint8_t glyph_col = 0; glyph_col < SMALL_CHAR_WIDTH; glyph_col++) {
        // 中文: 取出当前字符的第 glyph_col 列 bitmap。
        // English: Read the bitmap for this glyph column.
        uint8_t col_data = get_small_glyph_column(character, glyph_col);

        for (uint8_t row = 0; row < MATRIX_NUM_ROWS; row++) {
            // 中文: font bitmap 使用 MSB 作为顶部像素，所以这里用 (7 - row) 取 bit。
            // English: The font bitmap uses the MSB as the top pixel, so use (7 - row) here.
            if (col_data & (1 << (7 - row))) {
                column_colour_data[MATRIX_NUM_ROWS - 1 - row] = colour;
            } else {
                column_colour_data[MATRIX_NUM_ROWS - 1 - row] = COLOUR_BLACK;
            }
        }

        // 中文: 把字符的这一列画到屏幕上的 x_position + glyph_col。
        // English: Draw this glyph column at x_position + glyph_col on the screen.
        ledmatrix_update_column(x_position + glyph_col, column_colour_data);
    }

}

uint8_t get_small_glyph_column(char c, uint8_t col)
{
    // 中文: 先把字符转成字体数组下标，再返回对应列。
    // English: First map the character to a font array index, then return the requested column.
    uint8_t index = char_to_glyph_index(c);
    return font_small[index][col];
}

uint8_t char_to_glyph_index(char c)
{
    // 中文: font_small/font_large 的前 26 项是 A-Z。
    // English: The first 26 entries of font_small/font_large are A-Z.
    if ('A' <= c && c <= 'Z')
    {
        return c - 'A';             // 中文: 'A'-'Z' 映射到 0-25。 / English: Maps 'A'-'Z' to 0-25.
    } 
    if ('a' <= c && c <= 'z')
    {
        return c - 'a';             // 中文: 'a'-'z' 同样映射到 0-25。 / English: Maps 'a'-'z' to 0-25.
    } 
    // 中文: 数字 0-9 存在字母后面，也就是下标 26-35。
    // English: Digits 0-9 are stored after the letters, at indexes 26-35.
    if ('0' <= c && c <= '9')
    {
        return (c - '0') + 26;      // 中文: '0'-'9' 映射到 26-35。 / English: Maps '0'-'9' to 26-35.
    } 
    if (c == ' ')
    {
        return 36;
    } 
    return 37; // 中文: 无法识别的字符用 '?' 作为 fallback。 / English: Unknown characters fall back to '?'.
}

void display_preview(uint8_t marks_in_current_char, uint8_t current_morse) {
    // 中文: 保存当前未提交字符的状态，供 redraw_displayed_characters() 画红色预览。
    // English: Save the in-progress character state so redraw_displayed_characters() can draw the red preview.
    preview_marks = marks_in_current_char;
    preview_morse = current_morse;

    // 中文: 输入新的 mark 时取消正在进行的滑动动画，直接重画当前画面。
    // English: When a new mark is entered, cancel any slide animation and redraw the current frame directly.
    char_Move = 0;
    screen_Active = 0;

    redraw_displayed_characters();
}

// 中文: 这个函数根据已提交字符、预览字符和动画偏移重画 LED matrix。
// English: This function redraws the LED matrix using committed characters, preview state, and animation offset.
void redraw_displayed_characters(void) {
    ledmatrix_clear(); // 中文: 先清空屏幕，避免旧字符残留。 / English: Clear first so old glyph columns do not remain.

    // 中文: 已提交字符最右边字符的起始 x 坐标，会随动画向左移动。
    // English: Starting x position of the rightmost committed character; it shifts left during animation.
    uint8_t rightmost_letter_x = SMALL_CHAR_RIGHT_X - char_Move; // char_Move 是字母移动的距离 / char_Move is the shift amount.

    // 中文: 默认尝试画出所有已保存的绿色已提交字符。
    // English: By default, try to draw all stored committed green characters.
    uint8_t max_green_chars_to_draw = displayed_char_count;

    // 中文: 如果有红色预览字符，绿色已提交字符需要让出最右侧位置。
    // English: If there is a red preview character, committed green characters leave the rightmost slot free.
    if (preview_marks > 0) { // preview_marks 表示当前有未提交字符 / preview_marks means there is an in-progress character.
        rightmost_letter_x = SMALL_CHAR_RIGHT_X - CHAR_DISPLAY_STEP;
        if (max_green_chars_to_draw > 3) {
            max_green_chars_to_draw = 3;
            // 中文: 当前 16 列屏幕最多能同时容纳 3 个绿色字符 + 1 个红色预览字符。
            // English: With a 16-column screen, this layout fits up to 3 green characters plus 1 red preview.
        }
    }

    // 中文: 只绘制最近 max_green_chars_to_draw 个已提交字符。
    // English: Draw only the most recent max_green_chars_to_draw committed characters.
    uint8_t start_index = displayed_char_count - max_green_chars_to_draw;
    // 中文: start_index 表示从 displayed_chars[] 的哪个下标开始画。
    // English: start_index is the first displayed_chars[] index to draw.
    for (uint8_t i= 0; i < max_green_chars_to_draw; i++) {
        uint8_t char_index = start_index + i;

        // 中文: 越新的字符越靠右；CHAR_DISPLAY_STEP 控制字符间距。
        // English: Newer characters are placed further right; CHAR_DISPLAY_STEP controls the spacing.
        uint8_t x = rightmost_letter_x - CHAR_DISPLAY_STEP * (max_green_chars_to_draw - 1 - i);
        draw_small_char(displayed_chars[char_index], x, displayed_colours[char_index]);
    }

    if (preview_marks > 0) {
    // 中文: 把当前 Morse pattern 解码成字符，并在最右侧用红色预览。
    // English: Decode the current Morse pattern and preview it in red at the right edge.
    char preview_char = morse_to_char(preview_morse);
    draw_small_char(preview_char, SMALL_CHAR_RIGHT_X, COLOUR_RED);
    }
}


void start_animation(void){
        // 中文: 设置动画为 active，并从 0 列移动开始。
        // English: Mark animation as active and start from a 0-column shift.
        screen_Active = 1;
        char_Move = 0;
}

// 中文: LED matrix 动画每个 tick 前进一步，让新字符逐列滑入屏幕。
// English: Advance the LED matrix animation by one step per tick so the new character slides in column by column.
void process_next_matrix_animation_step() {
    if(screen_Active) {
        // 中文: 每次动画 tick 增加 1 列移动。
        // English: Each animation tick increases the movement by one column.
        char_Move++;

        if (char_Move >= 4){
            // 中文: 小字符间距为 4 列，移动 4 列后动画完成。
            // English: The small-character step is 4 columns, so the animation finishes after shifting 4 columns.
            screen_Active = 0;
            char_Move = CHAR_DISPLAY_STEP; 
        }
        redraw_displayed_characters();
    }
}

void add_character(char character)
{
    add_character_with_colour(character, COLOUR_GREEN);
}

void add_character_with_colour(char character, uint8_t colour)
{
    if (displayed_char_count < MAX_DISPLAY_CHARS) {
        displayed_chars[displayed_char_count] = character;
        displayed_colours[displayed_char_count] = colour;
        displayed_char_count++;
    } else {
        for (uint8_t i = 0; i < MAX_DISPLAY_CHARS - 1; i++) {
            displayed_chars[i] = displayed_chars[i + 1];
            displayed_colours[i] = displayed_colours[i + 1];
        }
        displayed_chars[MAX_DISPLAY_CHARS - 1] = character;
        displayed_colours[MAX_DISPLAY_CHARS - 1] = colour;
    }

    preview_marks = 0;
    preview_morse = 1;
    start_animation();
    redraw_displayed_characters();
}