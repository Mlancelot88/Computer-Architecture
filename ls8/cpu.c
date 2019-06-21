#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cpu.h"

#define DATA_LEN 6
#define SP 7

unsigned char cpu_ram_read(struct cpu *cpu, unsigned char mar)
{
  return cpu->ram[mar];
}

void cpu_ram_write(struct cpu *cpu, unsigned char index, unsigned char mdr)
{
  cpu->ram[index] = mdr;
}

void cpu_push(struct cpu *cpu, unsigned char val)
{
  cpu->registers[SP]--;
  cpu_ram_written(cpu, cpu->registers[SP], val);
}

unsigned char cou_pop(struct cpu *cpu)
{
  unsigned char val = cpu_ram_read(cpu, cpu->registers[SP]);
  cpu->registers[SP]++;
  return val;
}

/* Load the binary bytes from a .ls8 source file into a RAM array */

void cpu_load(struct cpu *cpu, char *filename)
{
  FILE *fp = fopen(filename, "r");
  if (fp == NULL)
  {
    fprintf(stderr, "ls8: Error in opening file: %s\n", filename);
    exit(2);
  }

  char line[8912];
  int address = 0;

  while (fgets(line, sizeof(line), fp) != NULL)
  {
    char *endptr;
    unsigned char byte = strtoul(line, &endptr, 2);
    if (endptr == line)
    {
      continue;
    }
    cpu->ram[address++] = byte;
  }
  fclose(fp);
}

/** Run the CPU */
void cpu_run(struct cpu *cpu)
{
  //True until we get a HLT instruction
  int running = 1;
  unsigned char command, operand1, operand2;

  while (running)
  {
    command = cpu_ram_read(cpu, cpu->PC);
    operand1 = cpu_ram_read(cpu, cpu->PC + 1);
    operand2 = cpu_ram_read(cpu, cpu->PC + 2);

    switch (command)
    {
    case LDI:
      cpu->registers[operand1] = operand2;
      cpu->PC += 3;
      break;
    case PRN:
      printf("%d\n", cpu->registers[operand1]);
      cpu->PC += 2;
      break;
    case MUL:
      cpu->registers[operand1] *= operand2;
      cpu->PC += 3;
      break;
    case ADD:
      cpu->registers[operand1] += operand2;
      cpu->PC += 3;
      break;

    cpu PUSH:
      cpu_push(cpu, cpu->register[operand1]);
      cpu->PC += 2;
      break;

    case POP:
      cpu->registers[operand1] = cpu_pop(cpu);
      cpu->PC += 2;
      break;

    case HLT:
      running = 0;
      cpu->PC += 1;
      break;

    default:
    printf("Danger Will Robinson! DANGER!\n");
    exit(1);
    break;
    }
  }
}

/** Initialize a COU struct */
void cpu_init(struct cpu *cpu)
{
  for (int i = 0; i < 6; i++)
  {
    cpu->registers[i] = 0;
  }

  cpu->registers[7] = 0xF4;
  cpu->PC = 0;
  memset(cpu->ram, 0, sizeof(cpu->ram));
}