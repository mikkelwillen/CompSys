/*
  Memory elements and IO devices

  Memory resizes dynamically to accomodate usage.
  IO Devices are located at addresses 0x1000000000 to 0x1FFFFFFF.
  Three devices are implemented:
  0x10000000 Input quadword from standard input
  0x10000001 Input pseudo-random quadword
  0x00000002 Output quadword to standard output
*/

#include "wires.h"

struct memory;
typedef struct memory *mem_p;

mem_p memory_create();
void memory_destroy(mem_p);
void memory_read_from_file(mem_p, const char* filename);

// set a tracefile for validation of memory writes and input/output
void memory_tracefile(mem_p mem, const char* filename);

// Read quadword. We use little endian byte-addressing and support unaligned access
// If the address is to an input device, input will be performed instead of memory access
val memory_read(mem_p, val address, bool enable);

// Write quadword with new value at rising edge of clock.
// There are no internal forwarding from write to read in same clock period
// Little endian byte-addressing and unaligned access is supported
// If the address is to an output device, output will be performed instead of memory access
void memory_write(mem_p, val address, val value, bool wr_enable);

// read 10 bytes unaligned, for instruction fetch
void memory_read_into_buffer(mem_p, val address, val bytes[], bool enable);

// parse argument vector and load it into simulated argv area
void memory_load_argv(mem_p, int argc, char* argv[]);
