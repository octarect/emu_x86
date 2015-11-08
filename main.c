#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "emulator.h"
#include "emulator_function.h"
#include "instruction.h"

/* メモリ1MB */
#define MEMORY_SIZE (1024 * 1024)

/* プログラム開始アドレス */
#define PROGRAM_ORIGIN (0x7c00)

/* スタック開始アドレス */
#define STACK_BASE (0x7c00)

char* registers_name[] = {
  "EAX", "ECX", "EDX", "EBX", "ESP", "EBP", "ESI", "EDI"
};

/* Emulatorのメモリにバイナリファイルの内容を512バイトコピーする */
static void read_binary(Emulator* emu, const char* filename)
{
  FILE* binary;

  binary = fopen(filename, "rb");

  if (binary == NULL) {
    printf("%sファイルが開けません\n", filename);
    exit(1);
  }

  /* 機械語ファイルを読み込む(最大512B) */
  fread(emu->memory + PROGRAM_ORIGIN, 1, 0x200, binary);
  fclose(binary);
}

/* 汎用レジスタとプログラムカウンタの値を標準出力に出力する */
static void dump_registers(Emulator* emu)
{
  int i;

  printf("[REGISTERS]\n");
  for (i = 0; i < REGISTERS_COUNT; i++) {
    printf("%s = %08x\n", registers_name[i], emu->registers[i]);
  }

  printf("EIP = %08x\n", emu->eip);
}

static void dump_stack(Emulator* emu)
{
  uint32_t sp = emu->registers[ESP];

  printf("[STACK]\n");
  for (; sp < STACK_BASE; sp = sp + 0x04) {
    printf("%x: %08x\n", sp, emu->memory[sp]);
  }
}

/* エミュレータを作成する */
Emulator* create_emu(size_t size, uint32_t eip, uint32_t esp)
{
  Emulator* emu = malloc(sizeof(Emulator));
  emu->memory = malloc(size);

  /* 汎用レジスタの初期値を全て0にする */
  memset(emu->registers, 0, sizeof(emu->registers));

  /* レジスタの初期値を指定されたものにする */
  emu->eip = eip;
  emu->registers[ESP] = esp;

  return emu;
}

/* エミュレータを破棄する */
void destroy_emu(Emulator* emu)
{
  free(emu->memory);
  free(emu);
}

/* エミュレータを確保する */
int opt_remove_at(int argc, char* argv[], int index)
{
  if (index < 0 || argc <= index) {
    return argc;
  } else {
    int i = index;
    for (; i < argc - 1; i++) {
      argv[i] = argv[i + 1];
    }
    argv[i] = NULL;
    return argc - 1;
  }
}

int main(int argc, char* argv[])
{
  Emulator* emu;
  int i;
  int quiet = 0;

  i = 1;
  while (i < argc) {
    if (strcmp(argv[i], "-q") == 0) {
      quiet = 1;
      argc = opt_remove_at(argc, argv, i);
    } else {
      i++;
    }
  }

  /* 引数が1つでなければエラー */
  if (argc != 2) {
    printf("usage: px86 filename\n");
    return 1;
  }

  /* 命令セットの初期化 */
  init_instructions();

  /* エミュレータを作る. EIPとESPも引数にて指定 */
  emu = create_emu(MEMORY_SIZE, PROGRAM_ORIGIN, STACK_BASE);

  /* 引数で与えられたバイナリを読み込む */
  read_binary(emu, argv[1]);

  while (emu->eip < MEMORY_SIZE) {
    uint8_t code = get_code8(emu, 0);

    /* 現在のプログラムカウンタと実行されるバイナリを出力する */
    if (!quiet) {
      printf("EIP = %X, Code = %02X\n", emu->eip, code);
    }

    if (instructions[code] == NULL) {
      printf("\n\nNot Implemented: %x\n", code);
      break;
    }

    /* 命令の実行 */
    instructions[code](emu);

    printf("BX=%08X, DI=%08X, SP=%08X\n", get_register16(emu, BX), get_register16(emu, DI),
      get_register16(emu, SP));
    printf("ESP=%X\n", emu->registers[ESP]);

    /* EIPが0になったらプログラム終了 */
    if (emu->eip == 0x00) {
      printf("\n\nend of program.\n\n");
      break;
    }
  }

  dump_stack(emu);
  dump_registers(emu);
  destroy_emu(emu);

  return 0;
}
