#ifndef MODRM_H_
#define MODRM_H_

#include <stdint.h>

#include "emulator.h"

/* ModRMを表す構造体 */
typedef struct {
  uint8_t mod;

  /* opecodeとreg_indexは別名で同一 */
  union {
    uint8_t opecode;
    uint8_t reg_index;
  };

  uint8_t rm;

  /* SIBが必要なmod/rmの組み合わせの時に使う */
  uint8_t sib;

  union {
    int8_t disp8; /* disp8は符号付き整数 */
    int16_t disp16;
    uint32_t disp32;
  };
} ModRM;

/* ModRM, SIB, ディスプレースメントを解析する */
void parse_modrm32(Emulator* emu, ModRM* modrm);

/* ModRM解析16bit版 */
void parse_modrm16(Emulator* emu, ModRM* modrm);

/* ModRMの内容に基づきメモリの実効アドレスを計算する */
uint32_t calc_memory_address16(Emulator* emu, ModRM* modrm);
uint32_t calc_memory_address32(Emulator* emu, ModRM* modrm);

/* メモリ/レジスタアクセッサ 32bit版 */
uint32_t get_rm32(Emulator* emu, ModRM* modrm);
void set_rm32(Emulator* emu, ModRM* modrm, uint32_t value);
uint32_t get_r32(Emulator* emu, ModRM* modrm);
void set_r32(Emulator* emu, ModRM* modrm, uint32_t value);

/* 8ビット版 */
uint8_t get_rm8(Emulator* emu, ModRM* modrm);
void set_rm8(Emulator* emu, ModRM* modrm, uint8_t value);
uint8_t get_r8(Emulator* emu, ModRM* modrm);
void set_r8(Emulator* emu, ModRM* modrm, uint8_t value);

/* 16ビット版 */
uint16_t get_rm16(Emulator* emu, ModRM* modrm);
void set_rm16(Emulator* emu, ModRM* modrm, uint16_t value);
uint16_t get_r16(Emulator* emu, ModRM* modrm);
void set_r16(Emulator* emu, ModRM* modrm, uint16_t value);


#endif
