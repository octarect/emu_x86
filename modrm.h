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
    uint32_t disp32;
  };
} ModRM;

/* ModRM, SIB, ディスプレースメントを解析する */
void parse_modrm(Emulator* emu, ModRM* modrm, uint8_t nosib);

/* ModRMの内容に基づきメモリの実効アドレスを計算する */
uint32_t calc_memory_address(Emulator* emu, ModRM* modrm);

/* rm32のレジスタまたはメモリの32bit値を取得する */
uint32_t get_rm32(Emulator* emu, ModRM* modrm);

/* rm32レジスタまたはメモリの32bit値を取得する */
void set_rm32(Emulator* emu, ModRM* modrm, uint32_t value);

/* r32のレジスタの32bit値を取得する */
uint32_t get_r32(Emulator* emu, ModRM* modrm);

/* r32のレジスタの32bit値を設定する */
void set_r32(Emulator* emu, ModRM* modrm, uint32_t value);

/* 8ビット版 */
uint8_t get_rm8(Emulator* emu, ModRM* modrm);
void set_rm8(Emulator* emu, ModRM* modrm, uint8_t value);
uint8_t get_r8(Emulator* emu, ModRM* modrm);
void set_r8(Emulator* emu, ModRM* modrm, uint8_t value);

#endif
