/*
  Registers

  A model of the 16 registers in x86prime
*/

#include "wires.h"

struct registers;
typedef struct registers *reg_p;

reg_p regs_create();
void regs_destroy(reg_p);

// associate with a tracefile for verification
void regs_tracefile(reg_p regs, const char *filename);

val reg_read(reg_p regs, val reg_num);
void reg_write(reg_p regs, val reg_num, val value, bool wr_enable);
