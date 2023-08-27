#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "instruction.h"
#include "emulator.h"
#include "emulator_function.h"
#include "io.h"

#include "modrm.h"

instruction_func_t* instructions[256];

static void mov_r8_imm8(Emulator* emu) {
  uint8_t reg = get_code8(emu, 0) - 0xB0;
  set_register8(emu, reg, get_code8(emu, 1));
  emu->eip += 2;
}

static void mov_r32_imm32(Emulator* emu) {
  uint8_t reg = get_code8(emu, 0) - 0xB8;
  uint32_t value = get_code32(emu, 1);
  set_register32(emu, reg, value);
  emu->eip += 5;
}

static void mov_r8_rm8(Emulator* emu) {
  emu->eip += 1;
  ModRM modrm;
  parse_mordm(emu, &modrm);
  uint32_t rm8 = get_rm8(emu, &modrm);
  set_r8(emu, &modrm, rm8);
}

static void mov_r32_rm32(Emulator* emu) {
  emu->eip += 1;
  ModRM modrm;
  parse_mordm(emu, &modrm);
  uint32_t rm8 = get_rm32(emu, &modrm);
  set_r32(emu, &modrm, rm8);
}

static void add_rm32_r32(Emulator* emu) {
  emu->eip += 1;
  ModRM modrm;
  parse_mordm(emu, &modrm);
  uint32_t r32 = get_r32(emu, &modrm);
  uint32_t rm32 = get_rm32(emu, &modrm);
  set_rm32(emu, &modrm, rm32 + r32);
}

static void mov_rm8_r8(Emulator* emu) {
  emu->eip += 1;
  ModRM modrm;
  parse_mordm(emu, &modrm);
  uint32_t r8 = get_r8(emu, &modrm);
  set_rm8(emu, &modrm, r8);
}

static void mov_rm32_r32(Emulator* emu) {
  emu->eip += 1;
  ModRM modrm;
  parse_modrm(emu, &modrm);
  uint32_t r32 = get_r32(emu, &modrm);
  set_rm32(emu, &modrm, r32);
}

static void inc_r32(Emulator* emu) {
  uint8_t reg = get_code8(emu, 0) - 0x40;
  set_register32(emu, reg, get_register32(emu, reg) + 1);
  emu->eip += 1;
}

static void push_r32(Emulator* emu) {
  uint8_t reg = get_code8(emu, 0) - 0x50;
  push32(emu, get_register32(emu, reg));
  emu->eip += 1;
}

static void pop_r32(Emulator* emu) {
  uint8_t reg = get_code8(emu, 0) - 0x58;
  set_register32(emu, reg, pop32(emu));
  emu->eip += 1;
}

static void push_imm32(Emulator* emu) {
  uint32_t value = get_code32(emu, 1);
  push32(emu, value);
  emu->eip += 5;
}

static void push_imm8(Emulator* emu) {
  uint8_t value = get_code8(emu, 1);
  push32(emu, value);
  emu->eip += 2;
}