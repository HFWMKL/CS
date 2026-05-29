/*
 * morse.c
 *
 * Main file
 * 中文: 项目主程序，负责初始化硬件、处理输入、协调 LED/serial/matrix/seven-seg 输出。
 * English: Main program file; initialises hardware, handles input, and coordinates LED/serial/matrix/seven-seg output.
 *
 * Authors: Peter Sutton, Bradley Stone, Ryan Wang
 * Modified by <YOUR NAME HERE>, <YOUR STUDENT ID HERE>
 */ 
// morse.c
// ├── serialio.h      负责 serial input/output
// ├── terminalio.h    负责 terminal 光标、清屏等效果
// ├── ledmatrix.h     负责 LED matrix 底层控制
// ├── display.h       负责 splash screen 和字符显示
// └── encoding.h      负责 Morse code 到字符的转换

/* Definitions */
#include <stdint.h>
// 提供明确宽度的整数类型，比如：uint8_t uint16_t int8_t int16_t

#include <stdio.h>
// 让你可以用：printf() fgetc()
// 不过在 AVR 里，printf() 默认不是输出到电脑的 terminal。
// 是因为后面的 serialio.c 把 printf() 绑定到了 serial interface，所以你才能用它在 serial terminal 上输出文字。
#include <avr/io.h>
// 这个是 AVR 硬件寄存器相关的头文件。
#include <avr/interrupt.h>
// avr/interrupt.h 这个提供 interrupt 相关函数，比如：sei();enable global interrupts，打开全局中断。

// 中文: 自己创建的组件。
// English: User-created modules.
#include "timer0.h"
#include "joystick.h"
/* Internal Library Includes */
#include "serialio.h"
#include "terminalio.h"
#include "ledmatrix.h"
#include "display.h"
#include "encoding.h"
#include "sevenseg.h"//这里是加入了七段数码管
#include "ledmatrix.h"


// 中文: Button 参数设置，PB0/PB1/PB2 分别用于 dot/dash/submit。
// English: Button definitions. PB0/PB1/PB2 are used for dot/dash/submit.
/* Button definitions */
#define BUTTON0_MASK (1 << PB0)
#define BUTTON1_MASK (1 << PB1)
#define BUTTON2_MASK (1 << PB2)
#define BUTTONS_MASK (BUTTON0_MASK | BUTTON1_MASK | BUTTON2_MASK)

// 中文: IO board LED 设置，假设 8 个 LED 连接到 Port C。
// English: IO board LED setup, assuming the 8 LEDs are connected to Port C.
/* IO LED definitions - assuming LEDs are connected to Port C */
#define IO_LED_DDR DDRC
#define IO_LED_PORT PORTC
//原本这里定义了字母本体宽度，但是现在挪到了display.h #define SMALL_CHAR_WIDTH 3

// 中文: 动画 beat 队列大小，用来缓存 IO LED 的 high/low beat。
// English: Size of the animation beat queue used to buffer high/low beats for the IO LEDs.
// 中文: Morse 数字最长可能有 5 个 dash，再加 gap/submit，因此取 32 作为安全容量。
// English: Morse digits can contain up to 5 dashes plus gaps/submits, so 32 is a safe capacity.
#define ANIMATION_QUEUE_SIZE 32


#define SERIAL_NUM_COLS 80
#define SERIAL_NUM_ROWS 24
// 中文: 同步输入模式开关 S0，连接到 PD3。
// English: Synchronous input mode switch S0, connected to PD3.
#define SWITCH0_MASK (1 << PD3)
#define SWITCH0_PIN  PIND
#define SWITCH0_DDR  DDRD


/* Internal Function Declarations */
// 提前声明函数，如果 main() 里要调用这些函数，而函数定义在 main() 后面，就需要先声明。
void initialise_hardware(void);
void start_morse(void);
void start_splash_screen(void);
void handle_inputs(void);

// 中文: 本文件内部使用的 helper 函数声明。
// English: Helper function declarations used only inside this file.
static void update_io_leds(void);
static void append_beat(uint8_t on);
static void input_dot(void);
static void input_dash(void);
static void input_submit(void);

// 中文: 动画队列相关函数，用于把 Morse high/low beat 延迟输出到 IO LEDs。
// English: Animation queue helpers for delayed Morse high/low beat output on the IO LEDs.
static void enqueue_beat(uint8_t beat);
static void enqueue_beats(uint8_t beat, uint8_t count);
static void process_next_animated_beat(void);
static uint8_t animation_queue_is_empty(void);
static void update_seven_segment_display(void);

// 中文: Serial terminal 输出相关 helper。
// English: Helper functions for serial terminal output.
static void serial_preview_character(char character);
static void serial_commit_character(char character);
static void serial_advance_cursor(void);
static char uppercase_char(char character);
static void commit_character_outputs(char character);

// 中文: 同步/异步输入模式相关 helper。
// English: Helpers for synchronous/asynchronous input modes.
static uint8_t synModeEnabled(void);
static void async_inputs(void);
static void syn_inputs_1ms(void);



static void handle_serial_input(void);
static void input_serial_character(char character);
static void enqueue_morse_pattern(uint8_t morse_code);
static void clear_in_progress_character(void);

// 中文: 最近 8 个 Morse beat 的历史，直接显示到 Port C 的 IO LEDs。
// English: History of the most recent 8 Morse beats, displayed directly on Port C IO LEDs.
static uint8_t led_history = 0;

// 中文: 当前正在输入的字符已经输入了多少个 mark。
// English: Number of marks entered for the in-progress character.
uint8_t marks_in_current_char = 0;

// 中文: 连续 submit 次数，用于区分字符间隔和单词间隔。
// English: Number of consecutive submits, used to distinguish character and word gaps.
uint8_t submit_count = 0;

// 中文: 当前 Morse pattern，初始 1 作为 prefix，后续 dot=0 dash=1。
// English: Current Morse pattern; initial 1 is the prefix, then dot=0 and dash=1.
static uint8_t current_morse = 1; //这里就已经设置的了prefix 1


// 中文: beat_queue 是循环队列，用于保存即将动画输出到 IO LEDs 的 beat。
// English: beat_queue is a circular queue storing beats that will be animated onto the IO LEDs.
static uint8_t beat_queue[ANIMATION_QUEUE_SIZE];
// 中文: queue_start 指向下一个要取出的 beat，queue_end 指向下一个要写入的位置。
// English: queue_start points to the next beat to consume; queue_end points to the next write position.
static uint8_t queue_start = 0;
static uint8_t queue_end = 0;

// 中文: 已提交字符数量，seven-seg 左 digit 用它按 hex modulo 16 显示。
// English: Submitted character count; the left seven-seg digit displays it as hex modulo 16.
uint8_t submitted_character_count = 0;

// 中文: Serial terminal 当前光标位置，从 1 开始计数。
// English: Current serial terminal cursor position, counted from 1.
static uint8_t serial_col = 1;
static uint8_t serial_row = 1;



// 程序入口
// English: Program entry point.
int main(void)
{
    initialise_hardware();
    start_splash_screen();
    start_morse();
}

// 这个函数负责初始化硬件。
// English: Initialise all hardware peripherals used by the project.
void initialise_hardware(void)
{   
    // 这行是初始化 SPI。SPI 是 AVR 和 LED matrix 通信的方式。
    // 设置 AVR 为 SPI master，并用某个速度参数初始化 SPI。
    spi_setup_master(128); // init LED matrix
    // Setup serial port for 19200 baud communication
    // 设置 serial
    //communication，波特率是19200 baud，AVR 和电脑 terminal 之间以 19200 的速度通信。
    init_serial_stdio(19200);

    DDRB &= ~BUTTONS_MASK;
    //DDRB = DDRB & 1111 1000 -> PB0 = input PB1 = input PB2 = input

    IO_LED_DDR = 0xff;
    IO_LED_PORT = 0x00;
    //把全部port设置成了输出output,IO_LED_DDR = 0xFF -> DDRC = 0b11111111;

    timer0_init();// 中文: 初始化 Timer0。 / English: Initialise Timer0.

    sevenseg_init();// 中文: 初始化七段数码管。 / English: Initialise the seven-segment display.
    SWITCH0_DDR &= ~SWITCH0_MASK;// 中文: S0 设置为输入，用于同步模式。 / English: Configure S0 as input for sync mode.
    sei(); // enable global interrupts
}

//这部分函数负责开场欢迎界面
// English: Show the startup splash screen and wait for a button press.
void start_splash_screen(void)
{
    // draw sigil on LED matrix
    // 在 LED matrix 上显示一个 splash screen 图案
    start_splash_display();
    //把 terminal 光标移动到第 10 列，第 6 行
    move_terminal_cursor(10, 6);
    printf("CSSE%d AVR Project", 7201); // change if masters student
    move_terminal_cursor(10, 8);
    printf("\"Morse Code Emulator\"");
    move_terminal_cursor(10, 10);
    printf("%d, Semester %s", 2026, "One");
    move_terminal_cursor(10, 12);
    // "%ld" is "long decimal", since a student number is bigger than 2**16
    printf("By %s (%ld)", "Student Name", 48000000);
    
    // Wait until a button is pressed
    //记住一点PINX，后面X是多少就说明是哪一个端口
    while (!(PINB & BUTTONS_MASK))
    {
        ;
    }

    timer0_reset_animation_counter();
    ledmatrix_clear();
}

// 中文: 进入 Morse emulator 的主循环，持续处理输入和定时动画。
// English: Enter the Morse emulator main loop, continuously handling input and timed animation.
void start_morse(void){
    // Clear the serial terminal
    //清除启动页面
    clear_terminal();
        // Handle any button or key inputs
        //这就是运行我后面写的逻辑，这就是函数主体
    clear_terminal();
    while (1)
    {
        handle_inputs();
        if (timer0_consume_1ms()) {
            // 中文: 1ms tick 用于同步输入检测和 seven-seg multiplex refresh。
            // English: The 1 ms tick is used for sync input polling and seven-seg multiplex refresh.
            syn_inputs_1ms();
            sevenseg_refresh();
        }
        if (timer0_consume_animation_tick()) {
            // 中文: 100ms tick 用于 IO LED beat 动画和 LED matrix 滑动动画。
            // English: The 100 ms tick drives IO LED beat animation and LED matrix slide animation.
            process_next_animated_beat();
            process_next_matrix_animation_step();
        }
    }    
    // should never reach
}

void handle_inputs(void)
{
    /* ******** START HERE ********
    
    Read the button. Enter a mark if there is a rising edge on b0.
    A way to do this is to check if the previous b0 state is 0,
    and the current b0 state is a 1.
	(You will need to implement a method of tracking the previous b0 state.)
	Ensure that when you press a button to exit the splash screen,
	that this button press doesn't immediately trigger an input here.
    
    --. --- --- -.. / .-.. ..- -.-. -.-
    */
    
    // 中文: S0 未开启时使用异步按钮模式；同步模式由 syn_inputs_1ms() 在 1ms tick 中处理。
    // English: When S0 is off, use asynchronous button mode; synchronous mode is handled in syn_inputs_1ms().
    if (!synModeEnabled()) {
        async_inputs();
    }
    handle_serial_input();
    // S0 low: asynchronous mode.

}
static void async_inputs() {
    // 中文: previous_buttons 保存上一轮按钮状态，用于检测 rising edge。
    // English: previous_buttons stores the last button state so rising edges can be detected.
    static uint8_t previous_buttons = 0;
    uint8_t current_buttons = PINB & BUTTONS_MASK;
    uint8_t rising_edges = current_buttons & ~ previous_buttons;
        
    if ( rising_edges & BUTTON0_MASK) {
            input_dot();
        }
        if (rising_edges & BUTTON1_MASK) {
            input_dash();
        }
        if (rising_edges & BUTTON2_MASK) {
            input_submit();
        }
        previous_buttons = current_buttons;
}

static void syn_inputs_1ms(void) {
    // 中文: 同步模式只用 B0 的按下时间判断 dot/dash，用松开时间自动 submit。
    // English: Sync mode uses B0 press duration for dot/dash and release duration for auto-submit.
    static uint8_t previous_b0 = 0;//记录上一毫秒 B0 是不是按下的。
    static uint16_t pressed_ms = 0;//记录 B0 已经连续按下多少毫秒。
    static uint16_t released_ms = 0; //记录 B0 已经连续松开多少毫秒。
    static uint8_t auto_submit_stage = 0;//记录自动 submit 到哪一步了

    if (!synModeEnabled()) {
        previous_b0 = 0;
        pressed_ms = 0;
        released_ms = 0;
        auto_submit_stage = 0;
        return;
    }

    uint8_t current_b0 = (PINB & BUTTON0_MASK) != 0;
//当前是否按键(按键之后不做任何处理存储到) -> 刚刚是否按键（）
    if (current_b0) {
        if (pressed_ms < 60000) {
            pressed_ms++;
        }

        released_ms = 0;//松开时间清空
        auto_submit_stage = 0;//正在按下，submit记录清空
    } else { //current_b0 = 0，当前已经松开
        if (previous_b0) { //上一毫秒 B0按下
            if (pressed_ms < 200) { //按下时间小于200ms dot,反之dash
                input_dot();
            } else {
                input_dash();
            }

            pressed_ms = 0;//dot,dash记录完了，清空
            released_ms = 0;//dot,dash记录完了，清空
            auto_submit_stage = 0;//dot,dash记录完了，清空
        } else { //上一毫秒 B0 松开
            if (released_ms < 60000) {
                released_ms++;
            }
            //下面的条件是松开时间大于1000，有输入，还没提交
            if (released_ms >= 1000 &&
                auto_submit_stage == 0 &&
                marks_in_current_char > 0) {
                input_submit();
                auto_submit_stage = 1;
            } else if (released_ms >= 2000 && auto_submit_stage == 1) {
                //松开时间大于2000，有提交
                input_submit();
                    auto_submit_stage = 2;
                }
            }
    }
        previous_b0 = current_b0;//现在状态存入上一个状态
}

static void handle_serial_input(void)
{
    while (serial_input_available()) {
        char character = fgetc(stdin);
        input_serial_character(character);
    }
}


static void update_io_leds(void)
{
    // 中文: 把 led_history 的 8 bit 直接输出到 Port C LEDs。
    // English: Output the 8 bits of led_history directly to the Port C LEDs.
    IO_LED_PORT = led_history;
}

static void append_beat(uint8_t on)
{
    // 中文: 左移历史并加入最新 beat；最新 beat 显示在最右侧 LED。
    // English: Shift the history left and insert the newest beat; newest beat appears on the rightmost LED.
    led_history = (led_history << 1) | (on ? 1:0);
    update_io_leds();
}

static void input_dot(void) {
    // 中文: 输入 dot：如果不是当前字符的第一个 mark，先插入 1 beat gap。
    // English: Input a dot: if it is not the first mark, insert a 1-beat gap first.
    uint8_t was_empty = animation_queue_is_empty();
    if (marks_in_current_char > 0) {
        enqueue_beat(0);//make a gap,one beat
    }
    enqueue_beat(1); //dot
    current_morse <<= 1;//dot = 0

    marks_in_current_char++;
    submit_count = 0;
    char decoded_char = morse_to_char(current_morse);
    serial_preview_character(decoded_char);
    update_seven_segment_display();
    //marks_in_current_char改变了，所以这里也要更新

    display_preview(marks_in_current_char, current_morse);//submit之前添加不完整的字母
}

static void input_dash(void) {
    // 中文: 输入 dash：dash 是 3 个 high beat，mark 之间仍需要 1 beat gap。
    // English: Input a dash: a dash is 3 high beats, with a 1-beat gap between marks.
    if (marks_in_current_char > 0) {
        enqueue_beat(0);
    }

    enqueue_beats(1,3);
    current_morse <<= 1; // 这里是prefix 1 左移位
    current_morse |= 1;  //这里是 current_morse = current_morse ｜ 1
    marks_in_current_char++;
    submit_count = 0;
    char decoded_char = morse_to_char(current_morse);
    serial_preview_character(decoded_char);
    update_seven_segment_display();
    //marks_in_current_char改变了，所以这里也要更新

    display_preview(marks_in_current_char, current_morse);//submit之前添加不完整的字母
}

static void input_submit(void) {
    // 中文: 第一次 submit 提交当前字符；第二次连续 submit 代表单词间隔。
    // English: First submit commits the current character; the second consecutive submit represents a word gap.
    if (marks_in_current_char > 0 && submit_count == 0) {
        //这里表示添加三次append_beat(0)
        enqueue_beats(0,3);
        //下面这两个函数是我任务二用到的
        char decoded_char = morse_to_char(current_morse);
        //这里是把Morse的数字转换成字符
        
        //  这个函数直接提交数据到terminal和LED显示
        commit_character_outputs(decoded_char);
        
        marks_in_current_char=0;
        submit_count = 1;
        current_morse = 1;
        submitted_character_count++;
        update_seven_segment_display();

    }else if (submit_count == 1) {
        //队列中添加两个append_beat(0)
        enqueue_beats(0,2);
        //这里是显示空格，绿色，这里是我任务二用到的
        submit_count = 2;
        commit_character_outputs(' ');
        //submitted_character_count++;
        update_seven_segment_display();
        //marks_in_current_char改变了，所以这里也要更新
        submit_count = 2;
    }else {}
}



static void input_serial_character(char character)
{
    char output_char = uppercase_char(character);
    uint8_t morse_code = char_to_morse(output_char);

    if (morse_code == 0) {
        return;
    }

    if (marks_in_current_char > 0) {
        clear_in_progress_character();
    }

    enqueue_morse_pattern(morse_code);

    add_character_with_colour(output_char, COLOUR_YELLOW);
    serial_commit_character(output_char);

    submitted_character_count++;
    update_seven_segment_display();
}




//这个函数是判断列表是否为空
static uint8_t animation_queue_is_empty(void)
{
    // 中文: 循环队列 head/tail 相同表示没有待处理 beat。
    // English: The circular queue is empty when head and tail are equal.
    return queue_start == queue_end;
}

//这里是我们添加append_beat到队列中
// English: Add one beat to the circular animation queue.
static void enqueue_beat(uint8_t beat) {
    uint8_t next_end = (queue_end + 1) % ANIMATION_QUEUE_SIZE;
    if (next_end != queue_start) {
        beat_queue[queue_end] = beat?1:0;
        queue_end = next_end;
    }
}

//这里是我们直接灵活调用，直接两个参数，调用可以一次性添加多个相同beat的append_beat
// English: Add several identical beats to the circular animation queue.
static void enqueue_beats(uint8_t beat, uint8_t count) {
    for (uint8_t i = 0; i < count; i++) {
        enqueue_beat(beat);
    }
}

static void update_seven_segment_display(void)
{
    // 中文: seven-seg 左边显示 submitted_character_count，右边显示 marks_in_current_char。
    // English: Seven-seg left shows submitted_character_count; right shows marks_in_current_char.
    sevenseg_update_state(submitted_character_count, marks_in_current_char);
}

static void process_next_animated_beat(void) {
    // 中文: 每个 animation tick 从队列取出一个 beat 并显示到 IO LEDs。
    // English: On each animation tick, consume one beat from the queue and show it on the IO LEDs.
    if (queue_start != queue_end) {
        uint8_t beat = beat_queue[queue_start];
        queue_start = (queue_start + 1) % ANIMATION_QUEUE_SIZE;
        append_beat(beat);
    }
}

static char uppercase_char(char character) {
    // 中文: Serial terminal 输出统一转换成大写字母。
    // English: Convert letters to uppercase for serial terminal output.
    if ('a' <= character && character <= 'z') {
        return character - ('a' - 'A');
    }

    return character;
}

//terminal输出
// English: Move the serial terminal cursor to the next printable position.
static void serial_advance_cursor(void) {
    serial_col++;

    if (serial_col > SERIAL_NUM_COLS) {
        serial_col = 1;
        serial_row++;
        
        if (serial_row > SERIAL_NUM_ROWS) {
            serial_row = 1;
            clear_terminal();
        }
    }

}

//下面这个函数负责解析完字符后提交给terminal
// English: Commit a decoded character to the serial terminal and advance the cursor.
static void serial_commit_character(char character) {
    character = uppercase_char(character);
    move_terminal_cursor(serial_col, serial_row);
    printf("%c", character);

    serial_advance_cursor();

    move_terminal_cursor(serial_col,serial_row);

}

//预输出
// English: Preview the current decoded character at the current cursor position without advancing.
static void serial_preview_character(char character) {
    char preview_char = uppercase_char(character);
    move_terminal_cursor(serial_col, serial_row);

    printf("%c", preview_char);

    move_terminal_cursor(serial_col, serial_row);
}

//这里相当于一口气提交数据到另外两个函数
// English: Send one committed character to both LED matrix output and serial terminal output.
static void commit_character_outputs(char character)
{
    add_character(character);          // LED matrix output
    serial_commit_character(character); // Serial terminal output
}
//打开syn模式
// English: Return non-zero when S0/synchronous input mode is enabled.
static uint8_t synModeEnabled(void)
{
    return (SWITCH0_PIN & SWITCH0_MASK) != 0;
}


static void clear_in_progress_character(void)
{
    marks_in_current_char = 0;
    submit_count = 0;
    current_morse = 1;

    display_preview(marks_in_current_char, current_morse);
    update_seven_segment_display();
}

static void enqueue_morse_pattern(uint8_t morse_code)
{
    int8_t first_mark_bit = -1;

    for (int8_t bit = 6; bit >= 0; bit--) {
        if (morse_code & (1 << bit)) {
            first_mark_bit = bit - 1;
            break;
        }
    }

    if (first_mark_bit < 0) {
        return;
    }

    for (int8_t bit = first_mark_bit; bit >= 0; bit--) {
        if (bit != first_mark_bit) {
            enqueue_beat(0);
        }

        if (morse_code & (1 << bit)) {
            enqueue_beats(1, 3);
        } else {
            enqueue_beat(1);
        }
    }

    enqueue_beats(0, 3);
}