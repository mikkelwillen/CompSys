/*
  Trace reader implementation
*/


#include <stdio.h>
#include <stdlib.h>

#include "support.h"
#include "trace_read.h"

typedef uint64_t word;

struct trace_reader  {
    FILE* trace_file;
    char filter;
    bool next_valid;
    word next_addr;
    word next_value;
};


trace_p trace_reader_create(char filter, const char* filename) {
    FILE* f = fopen(filename, "r");
    if (f == NULL)
        error("Failed to open tracefile");
    trace_p tracer = malloc(sizeof(struct trace_reader));
    tracer->trace_file = f;
    tracer->filter = filter;
    tracer->next_valid = false;
    return tracer;
}

void trace_reader_destroy(trace_p tracer) {
    fclose(tracer->trace_file);
    free(tracer);
}

void get_next_if_needed_and_possible(trace_p tracer) {
    if (tracer->next_valid) return; /* no need */
    int domain;
    do {
        domain = getc(tracer->trace_file); /* every line starts with the domain */
        if (domain == EOF) return; /* trace ended */
        int res = fscanf(tracer->trace_file, " %lx %lx ", &tracer->next_addr, &tracer->next_value);
        if (res != 2)
            error("Wrong trace format"); /* couldn't read the entry??? */
    } while (domain != tracer->filter); /* skip other domains */
    tracer->next_valid = true;
}

bool trace_match_next(trace_p tracer, val address, val value) {
    if (tracer == 0) return true; // no trace file - condition trivially true
    get_next_if_needed_and_possible(tracer);
    if (!tracer->next_valid) {
        printf(" -- end of tracefile reached, while trying to match '%c' %lx <- %lx\n", 
               tracer->filter, address.val, value.val);
        return false;
    }
    if (tracer->next_addr != address.val) {
        printf(" -- address mismatch, access '%c' %lx, but tracefile expected address: %lx\n", 
               tracer->filter, address.val, tracer->next_addr);
        return false;
    }
    if (tracer->next_value != value.val) {
        printf(" -- value mismatch, access '%c' %lx\n", tracer->filter, address.val);
        printf(" -- with value %lx, but tracefile expected %lx\n", value.val, tracer->next_value);
        return false;
    }
    /* matched! */
    tracer->next_valid = false; /* we just matched it */
    return true;
}

bool trace_match_and_get_next(trace_p tracer, val address, val* value) {
    if (tracer == 0) return true; // no trace file
    get_next_if_needed_and_possible(tracer);
    if (!tracer->next_valid) {
        printf(" -- end of tracefile reached, while trying to match '%c' %lx <- <<some value>>\n", 
               tracer->filter, address.val);
        return false;
    }
    if (tracer->next_addr != address.val) {
        printf(" -- address mismatch, access '%c' %lx, but tracefile expected address: %lx\n", 
               tracer->filter, address.val, tracer->next_addr);
        return false;
    }
    value->val = tracer->next_value;
    tracer->next_valid = false;
    return true;
}


bool trace_all_matched(trace_p tracer) {
    if (tracer == 0) return true; // no trace file - condition trivially true
    get_next_if_needed_and_possible(tracer);
    return !tracer->next_valid;
}

