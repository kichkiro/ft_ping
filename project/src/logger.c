/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kichkiro <kichkiro@student.42firenze.it    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 17:05:01 by kichkiro          #+#    #+#             */
/*   Updated: 2025/04/07 11:38:53 by kichkiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

void log_statistics(void) {
	char logs[1000];

	stat.pkt_loss = stat.pkt_loss * 100 / stat.pkts_tx;
	if (stat.pkt_loss != 100)
		snprintf(logs, sizeof(logs), "--- %s ping statistics ---\n" \
			"%d packets transmitted, %d packets received, %d%% packet loss\n"
			"round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n",
			stat.host, stat.pkts_tx, stat.pkts_rx, stat.pkt_loss, stat.rtt_min,
			stat.rtt_avg, stat.rtt_max, stat.rtt_stddev
		);
	else 
		snprintf(logs, sizeof(logs), "--- %s ping statistics ---\n" \
			"%d packets transmitted, %d packets received, %d%% packet loss\n",
			stat.host, stat.pkts_tx, stat.pkts_rx, stat.pkt_loss
		);
	logger(logs, INFO, true, EXIT_SUCCESS);
}

void log_run_ping(t_icmp_packet *req, struct sockaddr_in *dest_addr, bool v, \
	bool init, struct ip *ip_header, double rtt) {
	char logs[250];
	int len;

	memset(&logs, 0, (sizeof(logs)));
	len = 0;
	if (init) {
		if (v)
			snprintf(logs, sizeof(logs), "PING %s (%s): %ld data bytes, id 0x0%x = %d\n",
				stat.host, inet_ntoa(dest_addr->sin_addr),
				sizeof(req->payload), req->header.un.echo.id,
				req->header.un.echo.id
			);
		else
			snprintf(logs, sizeof(logs), "PING %s (%s): %ld data bytes\n",
				stat.host, inet_ntoa(dest_addr->sin_addr), sizeof(req->payload)
			);
		len = strlen(logs);
	}
	if (ip_header) {
		snprintf(logs + len, sizeof(logs), "%d bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n",
			ntohs(ip_header->ip_len) - ip_header->ip_hl * 4,
			inet_ntoa(dest_addr->sin_addr), req->header.un.echo.sequence,
			ip_header->ip_ttl, rtt
		);
	}
	logger(logs, INFO, false, 0);
}

void log_missing_host(void) {
	logger("ping: missing host operand\nTry 'ping --help' or 'ping --usage' for"
		" more information.\n", WARNING, true, 64);
}

void log_help(void) {
	logger("Usage: ping [OPTION...] HOST ...\n"\
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
		"Report bugs to <kichkiro@student.42firenze.it>.\n", INFO, true, 0
	);
}

void log_version(void) {
	logger("ping (42 School) 1.0\n"\
		"Copyright (C) 2025 Free Software Foundation, Inc.\n"\
		"License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>.\n"\
		"This is free software: you are free to change and redistribute it.\n"\
		"There is NO WARRANTY, to the extent permitted by law.\n\n"\
		"Written by kichkiro.\n", INFO, true, 0
	);
}

void log_usage(void) {
	logger("Usage: ping [-nrvf?V] [-T NUM] [-w N] [-W N] [-l NUMBER]\n"\
		"            [-p PATTERN] [-s NUMBER] [--numeric] [--ignore-routing]\n"\
		"            [--ttl=N] [--tos=NUM] [--verbose] [--timeout=N]\n"\
		"            [--linger=N] [--flood] [--ip-timestamp=FLAG]\n"\
		"            [--preload=NUMBER] [--pattern=PATTERN] [--size=NUMBER]\n"\
		"            [--help] [--usage] [--version]\n"\
		"            HOST ...\n", INFO, true, 0
	);
}

void log_invalid_option(char *raw) {
	char logs[140];

	if (!strncmp(raw, "--", 2))
		snprintf(logs, sizeof(logs), "ping: unrecognized option '%s'\n" \
			"Try 'ping --help' or 'ping --usage' for more information.\n", raw);
	else
		snprintf(logs, sizeof(logs), "ping: invalid option -- '%c'\nTry 'ping --help' or " \
			"'ping --usage' for more information.\n", raw[1]);
	logger(logs, ERROR, true, 1);
}

void log_missing_option_arg(char *raw) {
	char logs[140];

	if (strlen(raw) == 2)
		snprintf(logs, sizeof(logs), "ping: option requires an argument -- '%c'\nTry 'ping " \
			"--help' or 'ping --usage' for more information.\n", raw[1]);
	else
		snprintf(logs, sizeof(logs), "ping: option '%s' requires an argument\n" \
			"Try 'ping --help' or 'ping --usage' for more information.\n", raw);
	logger(logs, WARNING, true, 1);
}

void logger(char *msg, int level, bool to_exit, int exit_code) {
	char logs[500];
	int fd;

	if (msg && *msg) {
		if (level == DEBUG) {
			fd = 1;
			snprintf(logs, sizeof(logs), "%s", msg);
		}
		else if (level == INFO) {
			fd = 1;
			snprintf(logs, sizeof(logs), "%s", msg);
		}
		else if (level == WARNING) {
			fd = 2;
			snprintf(logs, sizeof(logs), "%s", msg);
		}
		else if (level == ERROR) {
			fd = 2;
			snprintf(logs, sizeof(logs), "%s", msg);
		}
		write(fd, logs, strlen(logs));
	}
	if (to_exit)
		exit(exit_code);
}
