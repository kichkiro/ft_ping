/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kichkiro <kichkiro@student.42firenze.it    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 17:05:01 by kichkiro          #+#    #+#             */
/*   Updated: 2025/07/03 22:05:47 by kichkiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

void log_stats(void) {
    char *logs;

    g.s.pkt_loss = g.s.pkt_loss * 100 / g.s.pkts_tx;
    if (g.s.pkt_loss != 100)
        logs = ft_strf(LOG_STAT LOG_STAT2, g.s.host, g.s.pkts_tx, g.s.pkts_rx, 
			g.s.pkt_loss, g.s.rtt_min, g.s.rtt_avg, g.s.rtt_max, g.s.rtt_stddev);
	else
        logs = ft_strf(LOG_STAT, g.s.host, g.s.pkts_tx, g.s.pkts_rx, g.s.pkt_loss);
    if (!logs)
        logger("log_stats(): internal_error", ERROR, true, EXIT_FAILURE);
    logger(logs, INFO, true, EXIT_SUCCESS);
}

static char *get_body(
    const t_pkt *resp, 
    const struct in_addr dst_ip, 
    const double rtt, 
    const t_opt opt
) {
    int pl_sz;
    const char *msg;

    pl_sz = ntohs(resp->ip_header->ip_len) - (resp->ip_header->ip_hl * 4);
    if (pl_sz < 0)
        pl_sz = 0;
    if (resp->icmp_pkt->header.type == ICMP_DEST_UNREACH) {
        if (resp->icmp_pkt->header.code == ICMP_NET_UNREACH)
            msg = "Destination Net Unreachable";
        else if (resp->icmp_pkt->header.code == ICMP_HOST_UNREACH)
            msg = "Destination Host Unreachable";
        else
            msg = "Destination Unreachable";
        return ft_strf(LOG_BYTES1, pl_sz, inet_ntoa(dst_ip), msg);
    } 
	else if (resp->icmp_pkt->header.type == ICMP_TIME_EXCEEDED)
        return ft_strf(LOG_BYTES2, pl_sz, inet_ntoa(dst_ip));
    else if (!opt.flood)
        return ft_strf(LOG_BYTES3, pl_sz, inet_ntoa(dst_ip), ntohs(resp->icmp_pkt->header.un.echo.sequence), resp->ip_header->ip_ttl, rtt);
}

void log_ping_body(
    const t_pkt *req, 
    const t_pkt *resp, 
    const struct in_addr dst_ip, 
    const double rtt, 
    const t_opt opt
) {
    char *basic;
    char *verbose;
    char *logs;

	verbose = NULL;
    if (!resp || !resp->icmp_pkt || !resp->ip_header)
        return;
    basic = get_body(resp, dst_ip, rtt, opt);
    if (opt.verbose && resp->icmp_pkt->header.type != ICMP_ECHOREPLY)
        verbose = get_verbose_body(resp, req);
    if (basic || verbose) {
        logs = ft_strf("%s%s", basic, verbose ? verbose : "");
        logger(logs, INFO, false, 0);
        free(basic);
        free(verbose);
        free(logs);
    }
}

void log_ping_hdr(const struct in_addr *dst_ip, const t_pkt *req) {
    char ip_buf[INET_ADDRSTRLEN];
    char *logs;
    size_t pl_sz;

    if (!req || !req->icmp_pkt)
        logger("log_ping_hdr(): internal error", ERROR, true, 1);
    if (!inet_ntop(AF_INET, dst_ip, ip_buf, sizeof(ip_buf)))
        logger("log_ping_hdr(): inet_ntop failed", ERROR, true, 1);
    pl_sz = sizeof(req->icmp_pkt->payload);
    if (!(logs = ft_strf(LOG_PING_HDR, g.s.host, ip_buf, pl_sz)))
        logger("log_ping_hdr(): internal error", ERROR, true, 1);
    logger(logs, INFO, false, 0);
    free(logs);
}

void log_mis_opt_arg(const char *raw) {
    char *logs;

    if (!raw || raw[0] == '\0') {
        logger(LOG_REQ_ARG1 LOG_TRY, WARNING, true, 1);
        return;
    }
    if (raw[0] == '-' && raw[1] == '-' && raw[2]) {
        if (!(logs = ft_strf(LOG_REQ_ARG2 LOG_TRY, raw)))
            logger("log_mis_opt_arg(): internal error", ERROR, true, 1);
    } 
	else {
        if (!(logs = ft_strf(LOG_REQ_ARG3 LOG_TRY, raw[1] ? raw[1] : '?')))
            logger("log_mis_opt_arg(): internal error", ERROR, true, 1);
    }
    logger(logs, WARNING, true, 1);
    free(logs);
}

void logger(
    char *msg, 
    const int level, 
    const bool to_exit, 
    const int exit_code
) {
    const char *lv;
    int fd;

    switch (level) {
        case DEBUG:   lv = "DEBUG";   fd = STDOUT_FILENO; break;
        case INFO:    lv = "INFO";    fd = STDOUT_FILENO; break;
        case WARNING: lv = "WARNING"; fd = STDERR_FILENO; break;
        case ERROR:   lv = "ERROR";   fd = STDERR_FILENO; break;
        default:      lv = "LOG";     fd = STDERR_FILENO; break;
    }
    if (msg && *msg)
        dprintf(fd, "%s", msg);
    if (to_exit) {
        ft_free((void **)&msg);
        exit(exit_code);
    }
}
