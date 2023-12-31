#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "emulator.h"
#include "emulator_function.h"
#include "instruction.h"

// メモリは1MB
#define MEMORY_SIZE (1024 * 1024)

char* registers_name[] = {"EAX", "ECX", "EDX", "EBX", "ESP", "EBP", "ESI", "EDI"};

// Emulatorのメモリにバイナリファイルの内容を512バイトコピーする
static void read_binary(Emulator* emu, const char* filename) {
  FILE *binary;

  binary = fopen(filename, "rb");

  if (binary == NULL) {
    printf("%s ファイルを開けません\n", filename);
    exit(1);
  }

  // Emulatorのメモリにバイナリファイルの内容を512バイトコピーする
  fread(emu->memory + 0x7c00, 1, 0x200, binary);
  fclose(binary);
}

// 汎用レジスタとプログラムカウンタの値を標準出力に出力する
static void dump_registers(Emulator* emu) {
  int i;

  for (i = 0; i < REGISTERS_COUNT; i++) {
    printf("%s = %08x\n", registers_name[i], get_register32(emu, i));
  }

  printf("EIP = %08x\n", emu->eip);
}

// 与えられた引数をもとにEmulatorを作成する
static Emulator* create_emu(size_t size, uint32_t eip, uint32_t esp) {
  // Emulatorの作成
  Emulator* emu = malloc(sizeof(Emulator));

  // Emulator内で使うメモリの確保
  emu->memory = malloc(size);

  // 汎用レジスタをすべて0にする
  memset(emu->registers, 0, sizeof(emu->registers));

  // プログラムカウンタの初期値
  emu->eip = eip;

  // スタックポインタの初期値
  emu->registers[ESP] = esp;

  return emu;
}

// エミュレータを破棄する
static void destroy_emu(Emulator* emu) {
  free(emu->memory);
  free(emu);
}

int opt_remove_at(int argc, char* argv[], int index) {
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

int main(int argc, char* argv[]) {
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

  // 引数が1つでなければエラーメッセージ
  if (argc != 2) {
    printf("usage: px86 filename\n");
    return 1;
  }

  // 命令セットの初期化を行う
  init_instructions();

  // メモリ1MBでEIP, ESPが0x7C00の状態のEmulatorを作る
  emu = create_emu(MEMORY_SIZE, 0x7c00, 0x7c00);

  // 引数で与えられたバイナリを読み込む
  read_binary(emu, argv[1]);

  while (emu->eip < MEMORY_SIZE) {
    uint8_t code = get_code8(emu, 0);
    // 現在のプログラムカウンタと実行されるバイナリを出力する
    if (!quiet) {
      printf("EIP = %X, Code = %02X\n", emu->eip, code);
    }

    if (instructions[code] == NULL) {
      // 実装されていない命令が来たらEmulatorを終了
      printf("\n\nNot Implemented: %x\n", code);
      break;
    }


    // 命令の実行
    instructions[code](emu);

    // EIP が0になったらプログラム終了
    if (emu->eip == 0) {
      printf("\n\nend of programs\n\n");
      break;
    }
  }

  dump_registers(emu);
  destroy_emu(emu);

  return 0;
}