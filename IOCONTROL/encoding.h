#ifndef ENCODING_H_
#define ENCODING_H_

#include <stdint.h>


// 中文: 把 Morse signal bit pattern 转换成对应字符。
// English: Converts a Morse signal bit pattern into the corresponding character.
// 中文: 如果 pattern 无效或未识别，返回 '?'。
// English: If the pattern is invalid or unrecognized, '?' is returned.
char morse_to_char(uint8_t code);
uint8_t char_to_morse(char character);
#endif /* ENCODING_H_ */
