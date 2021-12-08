#include "wires.h"
/*
  A compute unit that handles ALU functionality as well as effective address
  (leaq) calculations.
 */

// Encoding of ALU operations
#define ADD  0
#define SUB  1
#define AND  2
#define OR   3
#define XOR  4
#define MUL  5
#define SAR  6
#define SAL  7
#define SHR  8
#define IMUL 9

// Encoding of conditions
#define E  0x0
#define NE 0x1
#define L  0x4
#define LE 0x5
#define G  0x6
#define GE 0x7
#define A  0x8
#define AE 0x9
#define B  0xA
#define BE 0xB

bool comparator(val comparison, val op_a, val op_b);
val shifter(bool is_left, bool is_signed, val op_a, val op_b);
val multiplier(bool is_signed, val op_a, val op_b);
val alu_execute(val op, val op_a, val op_b);
val address_generate(val op_z_or_d, val op_s, val imm, val shift_amount,
		     bool sel_z_or_d, bool sel_s, bool sel_imm);




