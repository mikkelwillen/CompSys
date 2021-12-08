#include "arithmetic.h"

bool same_sign(val a, val b) {
    return ! (pick_one(63,a) ^ pick_one(63, b));
}

// For demonstration purposes we'll do addition without using the '+' operator :-)
// in the following, p == propagate, g == generate.
// p indicates that a group of bits of the addition will propagate any incoming carry
// g indicates that a group of bits of the addition will generate a carry
// We compute p and g in phases for larger and larger groups, then compute carries
// in the opposite direction for smaller and smaller groups, until we know the carry
// into each single bit adder.

typedef struct { val p; val g; } pg;

pg gen_pg(pg prev) {
    hilo p_prev = unzip(prev.p);
    hilo g_prev = unzip(prev.g);
    pg next;
    next.p = and( p_prev.lo, p_prev.hi);
    next.g = or( g_prev.hi, and( g_prev.lo, p_prev.hi));
    return next;
}

val gen_c(pg prev, val c_in) {
    hilo p_prev = unzip(prev.p);
    hilo g_prev = unzip(prev.g);
    hilo c_out;
    c_out.lo = c_in;
    c_out.hi = or( and(c_in, p_prev.lo), g_prev.lo);
    return zip(c_out);
}

generic_adder_result generic_adder(val val_a, val val_b, bool carry_in) {
    // determine p and g for single bit adds:
    pg pg_1;
    pg_1.p = or( val_a, val_b);
    pg_1.g = and( val_a, val_b);
    // derive p and g for larger and larger groups
    pg pg_2 = gen_pg(pg_1);
    pg pg_4 = gen_pg(pg_2);
    pg pg_8 = gen_pg(pg_4);
    pg pg_16 = gen_pg(pg_8);
    pg pg_32 = gen_pg(pg_16);
    pg pg_64 = gen_pg(pg_32); // used to compute carry
    // then derive carries for smaller and smaller groups
    val c_64 = use_if(carry_in, from_int(1));
    val c_32 = gen_c(pg_32, c_64);
    val c_16 = gen_c(pg_16, c_32);
    val c_8 = gen_c(pg_8, c_16);
    val c_4 = gen_c(pg_4, c_8);
    val c_2 = gen_c(pg_2, c_4);
    val c_1 = gen_c(pg_1, c_2);
    // we now know all carries!
    generic_adder_result result;
    result.result = xor( xor(val_a, val_b), c_1);

    // TODO: check that result.result == val_a + val_b + carry_in;
    // make the carry flag
    val cs = gen_c(pg_64, c_64);
    result.cf = pick_one(1, cs);
    return result;
}

val add(val a, val b) {
    generic_adder_result tmp;
    tmp = generic_adder(a, b, 0);
    return tmp.result;
}

val use_if(bool control, val value) {
    if (control) return value;
    else return from_int(0);
}

val and(val a, val b) {
    return from_int(a.val & b.val);
}

val or(val a, val b) {
    return from_int(a.val | b.val);
}

val xor(val a, val b) {
    return from_int(a.val ^ b.val);
}

val neg(int num_bits, val a) {
    if (num_bits < 64) {
        uint64_t mask = (((uint64_t) 1) << num_bits) - 1;
        return from_int(~a.val & mask);
    } else {
        return from_int(~a.val);
    }
}

bool is(uint64_t cnst, val a) {
    return a.val == cnst;
}

bool reduce_or(val a) {
    return a.val != 0;
}

bool reduce_and(int num_bits, val a) {
    return neg(num_bits, a).val == 0;
}
