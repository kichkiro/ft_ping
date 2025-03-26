/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kichkiro <kichkiro@student.42firenze.it    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 17:00:59 by kichkiro          #+#    #+#             */
/*   Updated: 2025/03/26 15:04:52 by kichkiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PING
# define FT_PING

// Libraries ------------------------------------------------------------------>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <time.h>
#include <netdb.h>

// Colors --------------------------------------------------------------------->
#define NO_COLOR "\033[0m"
#define BLUE     "\033[34m"
#define YELLOW   "\033[33m"
#define RED      "\033[31m"

// Log - Levels --------------------------------------------------------------->
#define DEBUG   1
#define INFO    2
#define WARNING 3
#define ERROR   4

// Log - Messagges ------------------------------------------------------------>
#define OPT_HELP "Try 'ping --help' or 'ping --usage' for more information.\n"
#define OPT_MISSING_HOST "ping: missing host operand\n"
#define OPT_MSG_HELP \
    "Usage: ping [OPTION...] HOST ...\n"\
    "Send ICMP ECHO_REQUEST packets to network hosts.\n\n"\
    " Options valid for all request types:\n\n"\
    "  -n, --numeric              do not resolve host addresses\n"\
    "  -r, --ignore-routing       send directly to a host on an attached network\n"\
    "      --ttl=N                specify N as time-to-live\n"\
    "  -T, --tos=NUM              set type of service (TOS) to NUM\n"\
    "  -v, --verbose              verbose output\n"\
    "  -w, --timeout=N            stop after N seconds\n"\
    "  -W, --linger=N             number of seconds to wait for response\n\n"\
    " Options valid for --echo requests:\n\n"\
    "  -f, --flood                flood ping (root only)\n"\
    "      --ip-timestamp=FLAG    IP timestamp of type FLAG, which is one of\n"\
    "                             \"tsonly\" and \"tsaddr\"\n"\
    "  -l, --preload=NUMBER       send NUMBER packets as fast as possible before\n"\
    "                             falling into normal mode of behavior (root only)\n"\
    "  -p, --pattern=PATTERN      fill ICMP packet with given pattern (hex)\n"\
    "  -s, --size=NUMBER          send NUMBER data octets\n\n"\
    "  -?, --help                 give this help list\n"\
    "      --usage                give a short usage message\n"\
    "  -V, --version              print program version\n\n"\
    "Mandatory or optional arguments to long options are also mandatory or optional\n"\
    "for any corresponding short options.\n\n"\
    "Options marked with (root only) are available only to superuser.\n\n"\
    "Report bugs to <kichkiro@student.42firenze.it>."
#define OPT_MSG_VERSION \
    "ping (42 School) 1.0\n"\
    "Copyright (C) 2025 Free Software Foundation, Inc.\n"\
    "License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>.\n"\
    "This is free software: you are free to change and redistribute it.\n"\
    "There is NO WARRANTY, to the extent permitted by law.\n\n"\
    "Written by kichkiro."
#define OPT_MSG_USAGE \
    "Usage: ping [-nrvf?V] [-T NUM] [-w N] [-W N] [-l NUMBER]\n"\
    "            [-p PATTERN] [-s NUMBER] [--numeric] [--ignore-routing]\n"\
    "            [--ttl=N] [--tos=NUM] [--verbose] [--timeout=N]\n"\
    "            [--linger=N] [--flood] [--ip-timestamp=FLAG]\n"\
    "            [--preload=NUMBER] [--pattern=PATTERN] [--size=NUMBER]\n"\
    "            [--help] [--usage] [--version]\n"\
    "            HOST ..."

// ICMP TYPE ------------------------------------------------------------------>
// #define ECHO_REPLY   0
// #define ECHO_REQUEST 8

// Structures ----------------------------------------------------------------->
typedef struct {
    bool set;
    int val;
} t_opt_preload;

typedef struct {
    bool set;
    int val;
} t_opt_timeout;

typedef struct {
    bool set;
    int val;
} t_opt_linger;

typedef struct {
    bool set;
    int val;
} t_opt_pattern;

typedef struct {
    bool set;
    int val;
} t_opt_size;

typedef struct {
    bool set;
    int val;
} t_opt_tos;

typedef struct {
    bool set;
    int val;
} t_opt_ttl;

typedef struct {
    bool set;
    int val;
} t_opt_iptimestamp;

typedef struct {
	bool verbose;
	bool flood;
	bool numeric;
	bool ignore_routing;
	t_opt_preload preload;
	t_opt_timeout timeout;
	t_opt_linger linger;
	t_opt_pattern pattern;
	t_opt_size size;
	t_opt_tos tos;
	t_opt_ttl ttl;
	t_opt_iptimestamp ip_timestamp;
} t_options;

typedef struct {
    struct in_addr host_ip;
    char *host_fqdn;
    t_options options;
} t_args;

typedef struct {
    uint8_t type;     
    uint8_t code;     
    uint16_t checksum;
    uint16_t id;      
    uint16_t sequence;
    char data[32];    
} t_icmp_pachet;

// Functions ------------------------------------------------------------------>
void ping(t_args *args);
void parser(char **raw, t_args *args);
void logger(char *msg, int level, bool to_exit, int exit_code);

#endif
