/*
  Implementation of x86prime 16 registers
*/

#include "ip_reg.h"
#include "support.h"
#include "trace_read.h"

#include <stdio.h>
#include <stdlib.h>

struct ip_register {
    trace_p tracer;
    val data;
};

ip_reg_p ip_reg_create() {
    ip_reg_p ip_reg = (ip_reg_p) malloc(sizeof(struct ip_register));
    ip_reg->data = from_int(0);
    ip_reg->tracer = 0;
    return ip_reg;
}

void ip_reg_destroy(ip_reg_p ip_reg) {
    if (ip_reg->tracer) {
        if (!trace_all_matched(ip_reg->tracer))
            error("Parts of trace for instruction pointer writes was not matched");
        trace_reader_destroy(ip_reg->tracer);
    }
    free(ip_reg);
}

void ip_reg_tracefile(ip_reg_p ip_reg, const char *filename) {
    ip_reg->tracer = trace_reader_create('P', filename);
}

val ip_read(ip_reg_p ip_reg) {
    return ip_reg->data;
}

void ip_write(ip_reg_p ip_reg, val value, bool wr_enable) {
    if (wr_enable) {
        if (trace_match_next(ip_reg->tracer, from_int(0), value))
            ip_reg->data = value;
        else
            error("Trace mismatch on write to instruction pointer");
    }
}
