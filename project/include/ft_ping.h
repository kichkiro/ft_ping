/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kichkiro <kichkiro@student.42firenze.it    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 17:00:59 by kichkiro          #+#    #+#             */
/*   Updated: 2025/04/08 15:51:44 by kichkiro         ###   ########.fr       */
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
    char *host;
    t_options options;
} t_args;

typedef struct {
    struct icmphdr header;
    char payload[56];
} t_icmp_packet;

typedef struct {
    struct ip *ip_header;
	struct icmp *icmp_header;
} t_packet;

typedef struct {
    char host[100];
    int pkts_tx;
    int pkts_rx;
    int pkt_loss;
    double rtt_min;
    double rtt_avg;
    double rtt_max;
    double rtt_stddev;
    double rtt_m2;
} t_statistics;

// Global Variables ----------------------------------------------------------->
extern t_statistics stat;

// Functions ------------------------------------------------------------------>
void ping(t_args *args);
void parser(char **raw, t_args *args);
void logger(char *msg, int level, bool to_exit, int exit_code);
void log_missing_option_arg(char *raw);
void log_invalid_option(char *raw);
void log_help(void);
void log_version(void);
void log_usage(void);
void log_missing_host(void);
void log_run_ping_init(t_icmp_packet *req, struct sockaddr_in *dest_addr,
	bool verbose);
void log_run_ping(t_icmp_packet *req, struct sockaddr_in *dest_addr, 
    t_packet response, double rtt, bool verbose);
void log_statistics(void);
size_t ft_int_len(int n);
size_t ft_str_realloc(char **ptr, size_t old_size, size_t add_size);

#endif
