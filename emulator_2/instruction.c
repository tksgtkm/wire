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

