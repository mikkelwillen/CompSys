/*
  Elementary functions for digital logic simulation

  All functions declared here corresponds to wire connections.
  No "real" computation is being carried out by them.

  A single wire, when used as a control signal, is represented by the type 'bool'
  Multiple wires, or a single wire not used as a control signal, is represented
  by the type 'val'

  For control signals (bools) you can use C built-in operators.
*/

#ifndef WIRES_H
#define WIRES_H

#include <inttypes.h>
#include <stdbool.h>

// A generic bitvector - max 64 bits, though. By accident sufficient for our needs :-)
typedef struct { uint64_t val; } val;

// A pair of bitvectors - used by zip() and unzip()
typedef struct {
    val hi;
    val lo;
} hilo;

// simple conversion
val from_int(uint64_t);

// unzip pairwise into two bitvectors holding even/odd bits
hilo unzip(val);

// zip a pair of bitvectors into one.
val zip(hilo);

// pick a set of bits from a value
// lsb: index of least significant bit
// sz: size
val pick_bits(int lsb, int sz, val);

// put sz least significant bits from val at position lsb in result.
// rest of result is 0.
val put_bits(int lsb, int sz, val);

// pick a single bit from a value
bool pick_one(int position, val);

// reverse the order of bytes within value
val reverse_bytes(int num_bytes, val value);

// sign extend by copying a sign bit to all higher positions
val sign_extend(int sign_position, val value);

#endif
