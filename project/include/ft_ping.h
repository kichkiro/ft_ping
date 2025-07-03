/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kichkiro <kichkiro@student.42firenze.it    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 17:00:59 by kichkiro          #+#    #+#             */
/*   Updated: 2025/07/03 22:04:54 by kichkiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PING
# define FT_PING

// Libraries ------------------------------------------------------------------>
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <signal.h>
#include <math.h>

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

// Log - Messages ------------------------------------------------------------->
#define LOG_CALLOC_FAIL "ping: calloc: memory allocation failed\n"
#define LOG_PING_HDR "PING %s (%s): %ld data bytes\n"
#define LOG_PING_HDR_VERB "PING %s (%s): %ld data bytes, id 0x0%x = %d\n"
#define LOG_TRY "Try 'ping --help' or 'ping --usage' for more information.\n"
#define LOG_MISS_HOST "ping: missing host operand\n"
#define LOG_REQ_ARG1 "ping: option requires an argument\n"
#define LOG_REQ_ARG2 "ping: option '%s' requires an argument\n"
#define LOG_REQ_ARG3 "ping: option requires an argument -- '%c'\n"
#define LOG_INVALID_OPT1 "ping: unrecognized option '%s'\n"
#define LOG_INVALID_OPT2 "ping: invalid option -- '%c'\n"
#define LOG_STAT "--- %s ping statistics ---\n"\
"%d packets transmitted, %d packets received, %d%% packet loss\n"
#define LOG_STAT2 "round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n"
#define LOG_BYTES1 "%d bytes from %s: %s\n"
#define LOG_BYTES2 "%d bytes from %s: Time to live exceeded\n"
#define LOG_BYTES3 "%d bytes from %s: icmp_seq=%u ttl=%d time=%.3f ms\n"
#define LOG_ICMP_HDR "ICMP: type %d, code %d, size %d, id 0x%04x, seq 0x%04x\n"
#define LOG_IP_HDR \
"Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src      Dst     Data\n"\
" %1d  %1d  %02x %4d %04x  %1d  %04x %3d  %02x %04x %-15s %-15s\n"
#define LOG_USAGE "Usage: ping [-nrvf?V] [-T NUM] [-w N] [-W N] [-l NUMBER]\n"\
"            [-p PATTERN] [-s NUMBER] [--numeric] [--ignore-routing]\n"\
"            [--ttl=N] [--tos=NUM] [--verbose] [--timeout=N]\n"\
"            [--linger=N] [--flood] [--ip-timestamp=FLAG]\n"\
"            [--preload=NUMBER] [--pattern=PATTERN] [--size=NUMBER]\n"\
"            [--help] [--usage] [--version]\n"\
"            HOST ...\n"
#define LOG_VERSION "ping (42 School) 1.0\n"\
"Copyright (C) 2025 Free Software Foundation, Inc.\n"\
"License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>.\n"\
"This is free software: you are free to change and redistribute it.\n"\
"There is NO WARRANTY, to the extent permitted by law.\n\n"\
"Written by kichkiro.\n"
#define LOG_HELP "Usage: ping [OPTION...] HOST ...\n"\
"Send ICMP ECHO_REQUEST packets to network hosts.\n\n"\
" Options valid for all request types:\n\n"\
"  -r, --ignore-routing       send directly to a host on an attached network\n"\
"      --ttl=N                specify N as time-to-live\n"\
"  -T, --tos=NUM              set type of service (TOS) to NUM\n"\
"  -v, --verbose              verbose output\n"\
"  -w, --timeout=N            stop after N seconds\n"\
"  -f, --flood                flood ping (root only)\n"\
"  -?, --help                 give this help list\n"\
"      --usage                give a short usage message\n"\
"  -V, --version              print program version\n\n"\
"Mandatory or optional arguments to long options are also mandatory or optional\n"\
"for any corresponding short options.\n\n"\
"Options marked with (root only) are available only to superuser.\n\n"\
"Report bugs to <kichkiro@student.42firenze.it>.\n"

// Structures ----------------------------------------------------------------->
// typedef struct {
//     bool set;
//     int val;
// } t_opt_preload;

typedef struct {
    bool set;
    int val;
} t_opt_timeout;

// typedef struct {
//     bool set;
//     int val;
// } t_opt_linger;

// typedef struct {
//     bool set;
//     int val;
// } t_opt_pattern;

// typedef struct {
//     bool set;
//     int val;
// } t_opt_size;

typedef struct {
    bool set;
    int val;
} t_opt_tos;

typedef struct {
    bool set;
    int val;
} t_opt_ttl;

// typedef struct {
//     bool set;
//     int val;
// } t_opt_iptimestamp;

typedef struct {
    bool verbose;
    bool flood;
    // bool numeric;
    bool ignore_routing;
    // t_opt_preload preload;
    t_opt_timeout timeout;
    // t_opt_linger linger;
    // t_opt_pattern pattern;
    // t_opt_size size;
    t_opt_tos tos;
    t_opt_ttl ttl;
    // t_opt_iptimestamp ip_timestamp;
} t_opt;

typedef struct {
    char *host;
    t_opt options;
} t_args;

typedef struct {
    struct icmphdr header;
    char payload[56];
} t_icmp_pkt;

typedef struct {
    struct ip *ip_header;
	t_icmp_pkt *icmp_pkt;
} t_pkt;

typedef struct {
    int send_fd;
    int recv_fd;
} t_sock;

typedef struct {
    char host[INET_ADDRSTRLEN];
    int pkts_tx;
    int pkts_rx;
    int pkt_loss;
    double rtt_min;
    double rtt_avg;
    double rtt_max;
    double rtt_stddev;
    double rtt_m2;
} t_stats;

typedef struct {
    t_sock *socks;
    t_pkt *req;
    t_pkt *resp;
} t_ptr;

typedef struct {
    t_stats s;
    t_ptr *p;
} t_g;

// Global Variables ----------------------------------------------------------->
extern t_g g;

// Functions ------------------------------------------------------------------>
void ping(const t_args *args);
void parser(char **raw, t_args *args);
t_sock *get_socket(const t_args *args);
void set_req_packet(
    t_pkt *req, 
    const struct in_addr dst_ip, 
    const bool init, 
    const t_opt opt
);
void recv_packet(const int fd, const struct sockaddr_in *dst_ip, t_pkt *resp);
void send_packet(const int fd, const struct sockaddr_in *dst_ip, t_pkt *req);
void logger(
    char *msg, 
    const int level, 
    const bool to_exit, 
    const int exit_code
);
void log_mis_opt_arg(const char *raw);
void log_ping_hdr(const struct in_addr *dst_ip, const t_pkt *req);
void log_ping_body(
    const t_pkt *req, 
    const t_pkt *resp, 
    const struct in_addr dst_ip, 
    const double rtt, 
    const t_opt opt
);
void log_stats(void);
void get_verbose_hdr(const struct in_addr *dst_ip, const t_pkt *req);
char *get_verbose_body(const t_pkt *resp, const t_pkt *req);
size_t ft_int_len(int n);
size_t ft_str_realloc(char **ptr, size_t old_size, size_t add_size);
char *ft_strf(const char *fmt, ...);
void ft_free(void **ptr);

#endif
