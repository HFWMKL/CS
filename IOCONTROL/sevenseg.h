// sevenseg.h
#ifndef SEVENSEG_H_
#define SEVENSEG_H_

#include <stdint.h>

// 中文: 初始化 seven-seg 的段选端口和位选端口。
// English: Initialise the seven-segment segment-select and digit-select ports.
void sevenseg_init(void);

// 中文: 更新 seven-seg 应该显示的逻辑状态，但不直接负责刷新扫描。
// English: Update the logical display state; this does not perform the multiplex refresh itself.
// 中文: subChCount 显示在左边，marks_in_current_char 显示在右边。
// English: subChCount is shown on the left; marks_in_current_char is shown on the right.
void sevenseg_update_state(uint8_t subChCount, uint8_t marks_in_current_char);

// 中文: 执行一次 multiplex refresh，轮流点亮左/右 digit。
// English: Perform one multiplex refresh step, alternating between left and right digits.
void sevenseg_refresh(void);


#endif
