/*
  IP Register
*/

#include "wires.h"

struct ip_register;
typedef struct ip_register *ip_reg_p;

ip_reg_p ip_reg_create();
void ip_reg_destroy(ip_reg_p);

// associate with a tracefile for validation
void ip_reg_tracefile(ip_reg_p reg, const char *filename);

val ip_read(ip_reg_p reg);
void ip_write(ip_reg_p reg, val value, bool wr_enable);
