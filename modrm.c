
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "modrm.h"
#include "emulator_function.h"

void parse_modrm(Emulator* emu, ModRM* modrm, uint8_t nosib)
{
  uint8_t code;

  assert(emu != NULL && modrm != NULL);

  memset(modrm, 0, sizeof(ModRM)); // 全部を 0 に初期化

  code = get_code8(emu, 0);
  modrm->mod = ((code & 0xC0) >> 6);
  modrm->opecode = ((code & 0x38) >> 3);
  modrm->rm = code & 0x07;

  emu->eip += 1;

  if (modrm->mod != 3 && modrm->rm == 4 && !nosib) {
    modrm->sib = get_code8(emu, 0);
    emu->eip += 1;
  }

  if ((modrm->mod == 0 && modrm->rm == 5) || modrm->mod == 2) {
    modrm->disp32 = get_sign_code32(emu, 0);
    emu->eip += 4;
  } else if (modrm->mod == 1) {
    modrm->disp8 = get_sign_code8(emu, 0);
    emu->eip += 1;
  }
}

void parse_modrm16(Emulator* emu, ModRM* modrm)
{
  uint8_t code;

  assert(emu != NULL && modrm != NULL);

  memset(modrm, 0, sizeof(ModRM)); // 全部を 0 に初期化

  code = get_code8(emu, 0);
  modrm->mod = ((code & 0xC0) >> 6);
  modrm->opecode = ((code & 0x38) >> 3);
  modrm->rm = code & 0x07;

  emu->eip += 1;

  if ((modrm->mod == 0 && modrm->rm == 6) || modrm->mod == 2) {
    modrm->disp16 = get_sign_code16(emu, 0);
    emu->eip += 2;
  } else if (modrm->mod == 1) {
    modrm->disp8 = get_sign_code8(emu, 0);
    emu->eip += 1;
  }
}

/* SIB評価(自己実装) */
uint32_t eval_sib(Emulator* emu, ModRM* modrm)
{
  uint8_t sib = modrm->sib;
  uint8_t scale = ((sib & 0xC0) >> 6);
  uint8_t base  = ((sib & 0x38) >> 3);
  uint8_t index = (sib & 0x03);
  uint32_t r32b, r32i;

  if (base == 5) {
    r32b = 0;
  } else {
    r32b = get_register32(emu, base);
  }

  if (index == 4) {
    return r32b;
  } else {
    r32i = get_register32(emu, index);
  }

  if (scale == 0) {
    return r32b;
  } else if (scale == 1) {
    return r32b + r32i * 2;
  } else if (scale == 2) {
    return r32b + r32i * 4;
  } else {  /* scale == 3 */
    return r32b + r32i * 8;
  }
}

uint32_t get_index_address(Emulator* emu, ModRM* modrm)
{
  if (modrm->rm == 0) {
    return (uint32_t)get_register16(emu, BX) + (uint32_t)get_register32(emu, SI);
  } else if (modrm->rm == 1) {
    return (uint32_t)get_register16(emu, BX) + (uint32_t)get_register32(emu, DI);
  } else if (modrm->rm == 2) {
    return (uint32_t)get_register16(emu, BP) + (uint32_t)get_register32(emu, SI);
  } else if (modrm->rm == 3) {
    return (uint32_t)get_register16(emu, BP) + (uint32_t)get_register32(emu, DI);
  } else if (modrm->rm == 4) {
    return (uint32_t)get_register16(emu, SI);
  } else if (modrm->rm == 5) {
    return (uint32_t)get_register16(emu, DI);
  } else if (modrm->rm == 6) {
    return (uint32_t)get_register16(emu, BP);
  } else if (modrm->rm == 7) {
    return (uint32_t)get_register16(emu, BX);
  }
}

uint32_t calc_memory_address16(Emulator* emu, ModRM* modrm)
{
  /* レジスタ間接参照型 */
  if (modrm->mod == 0) {
    if (modrm->mod == 6) {
      return modrm->disp16;
    } else {
      return get_index_address(emu, modrm);
    }
  /* レジスタ間接参照 + 8bitディスプレースメント型 */
  } else if (modrm->mod == 1) {
    return get_index_address(emu, modrm) + modrm->disp8;
  /* レジスタ間接参照 + 16bitディスプレースメント型 */
  } else if (modrm->mod == 2) {
    return get_index_address(emu, modrm) + modrm->disp16;
  /* レジスタ直接参照型 */
  } else {
    printf("not implemented ModRM mod\n");
    exit(0);
  }
}

uint32_t calc_memory_address(Emulator* emu, ModRM* modrm)
{
  /* レジスタ間接参照型 */
  if (modrm->mod == 0) {
    if (modrm->rm == 4) {
      return eval_sib(emu, modrm);
    } else if (modrm->mod == 5) {
      return modrm->disp32;
    } else {
      return get_register32(emu, modrm->rm);
    }
  /* レジスタ間接参照 + 8bitディスプレースメント型 */
  } else if (modrm->mod == 1) {
    if (modrm->rm == 4) {
      return eval_sib(emu, modrm) + modrm->disp8;
    } else {
      return get_register32(emu, modrm->rm) + modrm->disp8;
    }
  /* レジスタ間接参照 + 32bitディスプレースメント型 */
  } else if (modrm->mod == 2) {
    if (modrm->rm == 4) {
      return eval_sib(emu, modrm) + modrm->disp32;
    } else {
      return get_register32(emu, modrm->rm) + modrm->disp32;
    }
  /* レジスタ直接参照型 */
  } else {
    printf("not implemented ModRM mod\n");
    exit(0);
  }
}

uint8_t get_rm8(Emulator* emu, ModRM* modrm)
{
  if (modrm->mod == 3) {
    return get_register8(emu, modrm->rm);
  } else {
    uint32_t address = calc_memory_address(emu, modrm);
    return get_memory8(emu, address);
  }
}

uint16_t get_rm16(Emulator* emu, ModRM* modrm)
{
  if (modrm->mod == 3) {
    return get_register16(emu, modrm->rm);
  } else {
    uint32_t address = calc_memory_address16(emu, modrm);
    return get_memory16(emu, address);
  }
}

uint32_t get_rm32(Emulator* emu, ModRM* modrm)
{
  if (modrm->mod == 3) {
    return get_register32(emu, modrm->rm);
  } else {
    uint32_t address = calc_memory_address(emu, modrm);
    return get_memory32(emu, address);
  }
}

void set_rm8(Emulator* emu, ModRM* modrm, uint8_t value)
{
  if (modrm->mod == 3) {
    set_register8(emu, modrm->rm, value);
  } else {
    uint32_t address = calc_memory_address(emu, modrm);
    set_memory8(emu, address, value);
  }
}

void set_rm16(Emulator* emu, ModRM* modrm, uint16_t value)
{
  if (modrm->mod == 3) {
    set_register16(emu, modrm->rm, value);
  } else {
    uint32_t address = calc_memory_address16(emu, modrm);
    set_memory16(emu, address, value);
  }
}

void set_rm32(Emulator* emu, ModRM* modrm, uint32_t value)
{
  if (modrm->mod == 3) {
    set_register32(emu, modrm->rm, value);
  } else {
    uint32_t address = calc_memory_address(emu, modrm);
    set_memory32(emu, address, value);
  }
}

uint8_t get_r8(Emulator* emu, ModRM* modrm)
{
  return get_register8(emu, modrm->reg_index);
}

uint16_t get_r16(Emulator* emu, ModRM* modrm)
{
  return get_register16(emu, modrm->reg_index);
}

uint32_t get_r32(Emulator* emu, ModRM* modrm)
{
  return get_register32(emu, modrm->reg_index);
}

void set_r8(Emulator* emu, ModRM* modrm, uint8_t value)
{
  set_register8(emu, modrm->reg_index, value);
}

void set_r16(Emulator* emu, ModRM* modrm, uint16_t value)
{
  set_register16(emu, modrm->reg_index, value);
}

void set_r32(Emulator* emu, ModRM* modrm, uint32_t value)
{
  set_register32(emu, modrm->reg_index, value);
}
