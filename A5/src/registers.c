/*
  Implementation of x86prime 16 registers
*/

#include "registers.h"
#include "support.h"
#include "trace_read.h"

#include <stdio.h>
#include <stdlib.h>

struct registers {
    trace_p tracer;
    val data[16];
};

reg_p regs_create() {
    reg_p regs = (reg_p) malloc(sizeof(struct registers));
    for (int i = 0; i < 16; ++i)
        regs->data[i] = from_int(0);
    regs->tracer = 0;
    return regs;
}

void regs_destroy(reg_p regs) {
    if (regs->tracer) {
        if (!trace_all_matched(regs->tracer))
            error("Parts of trace for register writes was not matched");
        trace_reader_destroy(regs->tracer);
    }
    free(regs);
}

void regs_tracefile(reg_p regs, const char *filename) {
    regs->tracer = trace_reader_create('R', filename);
}

val reg_read(reg_p regs, val reg_num) {
    return regs->data[reg_num.val & 0x0F];
}

void reg_write(reg_p regs, val reg_num, val value, bool wr_enable) {
    if (wr_enable) {
        if (trace_match_next(regs->tracer, reg_num, value))
            regs->data[reg_num.val & 0x0F] = value;
        else
            error("Trace mismatch on write to register");
    }
}
