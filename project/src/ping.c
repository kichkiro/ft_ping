/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kichkiro <kichkiro@student.42firenze.it    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 13:39:41 by kichkiro          #+#    #+#             */
/*   Updated: 2025/07/03 22:10:55 by kichkiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

t_g g;

static void set_rtt_stats(const t_pkt *resp, double rtt) {
    if (resp->ip_header) {
        g.s.rtt_min = (rtt < g.s.rtt_min || !g.s.rtt_min) ? rtt : g.s.rtt_min;
        g.s.rtt_avg = (g.s.rtt_avg * (g.s.pkts_rx - 1) + rtt) / g.s.pkts_rx;
        g.s.rtt_max = (rtt > g.s.rtt_max) ? rtt : g.s.rtt_max;
        g.s.rtt_m2 = (g.s.rtt_m2 * (g.s.pkts_rx - 1) + rtt * rtt) / g.s.pkts_rx;
        g.s.rtt_stddev = sqrt(g.s.rtt_m2 - g.s.rtt_avg * g.s.rtt_avg);
    }
}

static void free_pkt(t_pkt *req, t_pkt *resp) {
	ft_free((void **)&req->icmp_pkt);
	g.p->req->icmp_pkt = NULL;
	ft_free((void **)&req->ip_header);
	g.p->req->ip_header = NULL;
	ft_free((void **)&req);
	g.p->req = NULL;
	ft_free((void **)&resp->ip_header);
	g.p->resp->ip_header = NULL;
	ft_free((void **)&resp);
	g.p->resp = NULL;
}

static void run(
	const t_sock *socks,
	const t_args *args, 
	const struct sockaddr_in dst_ip, 
	const bool init
) {
	t_pkt *req;
	t_pkt *resp;
	struct timeval ts_start, ts_end;
	double rtt;

	if (!(req = (t_pkt *)calloc(1, sizeof(t_pkt))))
		logger(LOG_CALLOC_FAIL, ERROR, true, 1);
	g.p->req = req;
	if (!(resp = (t_pkt *)calloc(1, sizeof(t_pkt))))
		logger(LOG_CALLOC_FAIL, ERROR, true, 1);
	g.p->resp = resp;
	set_req_packet(req, dst_ip.sin_addr, init, args->options);
	if (init && args->options.verbose)
		get_verbose_hdr(&dst_ip.sin_addr, req);
	else if (init)
		log_ping_hdr(&dst_ip.sin_addr, req);
	gettimeofday(&ts_start, 0);	
	send_packet(socks->send_fd, &dst_ip, req);
	recv_packet(socks->recv_fd, &dst_ip, resp);
	gettimeofday(&ts_end, 0);
    rtt = ((ts_end.tv_sec - ts_start.tv_sec) * 1000) + \
        ((ts_end.tv_usec - ts_start.tv_usec) / 1000.0);
    set_rtt_stats(resp, rtt);
	log_ping_body(req, resp, dst_ip.sin_addr, rtt, args->options);
	free_pkt(req, resp);
    if (!args->options.flood)
        usleep(1000000 - (rtt * 1000));
}

static void set_dst_ip(struct sockaddr_in *dst_ip, const t_args *args) {
	struct hostent *he;

    memset(dst_ip, 0, sizeof(*dst_ip));
	dst_ip->sin_family = AF_INET;
	if (args->host) {
		if (!(he = gethostbyname(args->host)))
			logger("ping: unknown host\n", ERROR, true, 1);
		dst_ip->sin_addr = *((struct in_addr *)he->h_addr_list[0]);
	}
}

void ping(const t_args *args) {
    t_sock *socks;
    double elapsed;
	struct sockaddr_in dst_ip;
	struct timeval ts_now, ts_start;
	bool init;

	set_dst_ip(&dst_ip, args);
    socks = get_socket(args);
	g.p->socks = socks;
	gettimeofday(&ts_start, NULL);
	init = true;
	while (true) {
		if (args->options.timeout.set) {
			gettimeofday(&ts_now, NULL);
			elapsed = (ts_now.tv_sec - ts_start.tv_sec) +
				(ts_now.tv_usec - ts_start.tv_usec) / 1e6;
                if (elapsed >= args->options.timeout.val) {    
                    log_stats();
                    break;
                }
		}
		run(socks, args, dst_ip, init);
		init = false;
	}
	close(socks->send_fd);
	close(socks->recv_fd);
}
