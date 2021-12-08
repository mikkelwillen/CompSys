/*
  trace reader. Used by memories to validate changes against a trace built by x86prime
*/

#include "wires.h"

struct trace_reader;

typedef struct trace_reader *trace_p;

trace_p trace_reader_create(char filter, const char* filename);
void trace_reader_destroy(trace_p tracer);

bool trace_match_next(trace_p tracer, val address, val value); // true if matched
bool trace_match_and_get_next(trace_p tracer, val address, val* value); // true if matched AND enabled

bool trace_all_matched(trace_p tracer);
