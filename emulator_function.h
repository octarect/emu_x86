#ifndef EMULATOR_FUNCTION_H_
#define EMULATOR_FUNCTION_H_
#include <stdint.h>

#include "emulator.h"

/* プログラムカウンタから相対位置にある符号無し8bit値を取得 */
uint32_t get_code8(Emulator* emu, int index);

/* プログラムカウンタから相対位置にある符号付き8bit値を取得 */
uint32_t get_sign_code8(Emulator* emu, int index);

/* プログラムカウンタから相対位置にある符号無し32bit値を取得 */
uint32_t get_code32(Emulator* emu, int index);

/* プログラムカウンタから相対位置にある符号付き32bit値を取得 */
uint32_t get_sign_code32(Emulator* emu, int index);

#endif
