#pragma once
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "csapp.h"

/*
 * add macros and function declarations which should be 
 * common between the name server and peer programs in this file.
 */

#define USERNAME_LEN       32   // max length of a valid username.
#define MAX_LINE           128  // max length of a line of user input.
#define MAX_USER_ARGNUM 4       // max number of arguments to peer commands.

#define IP_LEN             16
#define PORT_LEN           8

/* slightly less awkward string equality check */
#define string_equal(x, y) (strncmp((x), (y), strlen(y)) == 0)

/* enum type used to represent client commands */
typedef enum command_t {
  LOGIN,
  LOGOUT,
  EXIT,
  LOOKUP,
  MSG,
  SHOW,
  ERROR
} command_t;

/* string array type of size MAX_USER_ARGNUM used to hold arguments to a command */
typedef char *args_t[MAX_USER_ARGNUM];


/*
 * given "input", a newline- or NULL-terminated line of
 * user input containing command, followed by 0 or more
 * space-separated arguments, and "args", an args_t array:
 * - parses command and stores arguments sequentially in args.
 * - returns a command_t code representing the command.
 * - on an invalid number of arguments, ERROR is returned.
 *
 *   example: if command is "/login anders topsecret 130.225.245.178 1337", then
 *   args is the array ["anders", "topsecret", "130.225.245.178", "1337"],
 *   and the function returns LOGIN.
 *   (if command is a /msg, then recipient username and actual message stored in args[0..1]).
 */
command_t parse_command(char *input,
                        args_t args);

/*
 * extract arguments from a line of user input and store in args.
 */
size_t extract_args(char *input,
                    args_t args);


/*
 * naive validity checks of IP addresses and port numbers given as strings,
 * eg. at command line. both return zero on invalid IP/port, else non-zero.
 */
int is_valid_ip(char *ip_string);
int is_valid_port(char *port_string);
