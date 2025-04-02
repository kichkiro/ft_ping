/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kichkiro <kichkiro@student.42firenze.it    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 13:39:41 by kichkiro          #+#    #+#             */
/*   Updated: 2025/04/02 16:14:29 by kichkiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

t_statistics stat;

static unsigned short checksum(void *b, int len) {
	unsigned short *buf;
	unsigned int sum;

	buf = b;
	sum = 0;
	while (len > 1) {
		sum += *buf++;
		len -= 2;
	}
	if (len == 1)
		sum += *((uint8_t *)buf);
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	return (unsigned short)~sum;
}

static struct ip *recv_packet(int serv_fd, struct sockaddr_in *dest_addr, ssize_t *bytes) {
	struct ip *ip_header;
	char rbuf[1024];
	socklen_t addr_len;

	memset(&ip_header, 0, sizeof(ip_header));
	addr_len = sizeof(*dest_addr);
	if ((*bytes = recvfrom(serv_fd, rbuf, sizeof(rbuf), 0,
						   (struct sockaddr *)dest_addr, &addr_len)) <= 0)
		stat.pkt_loss++; 
	else {
		stat.pkts_rx++;
		ip_header = ((struct ip *)rbuf);
	}
	return ip_header;
}

static void send_packet(int serv_fd, struct sockaddr_in *dest_addr, t_icmp_packet *req) {
	if (sendto(serv_fd, req, sizeof(*req), 0,
			   (struct sockaddr *)dest_addr, sizeof(*dest_addr)) <= 0)
		logger("ping: send failed", ERROR, true, 1);
	else
		stat.pkts_tx++;
}

static void set_packet(t_icmp_packet *req, bool init) {
	struct timeval ts;
	static char data[40];

	if (init) {
		for (int i = 0; i < 40; i++)
			data[i] = (unsigned char)i;
		memset(req, 0, sizeof(*req));
		req->header.type = ICMP_ECHO;
		req->header.code = 0;
		req->header.un.echo.id = getpid();
		req->header.un.echo.sequence = 0;
	}
	else
		req->header.un.echo.sequence++;
	memset(&ts, 0, sizeof(ts));
	gettimeofday(&ts, 0);
	memset(req->payload, 0, 64);
	memcpy(req->payload, &ts, sizeof(ts));
	memcpy(req->payload + sizeof(ts), &data, sizeof(data));
	req->header.checksum = 0;
	req->header.checksum = checksum(req, sizeof(*req));
}

static void run(int serv_fd, t_args *args, struct sockaddr_in dest_addr, bool init) {
	t_icmp_packet req;
	struct ip *ip_header;
	struct timeval ts_start, ts_end;
	double rtt;
	ssize_t bytes_recv;

	set_packet(&req, init);
	gettimeofday(&ts_start, 0);
	send_packet(serv_fd, &dest_addr, &req);
	ip_header = recv_packet(serv_fd, &dest_addr, &bytes_recv);
	gettimeofday(&ts_end, 0);
	rtt = ((ts_end.tv_sec - ts_start.tv_sec) * 1000) + \
		((ts_end.tv_usec - ts_start.tv_usec) / 1000.0);
	if (ip_header) {
		stat.rtt_min = (rtt < stat.rtt_min || !stat.rtt_min) ? rtt : stat.rtt_min; 
		stat.rtt_avg = (stat.rtt_avg * (stat.pkts_rx - 1) + rtt) / stat.pkts_rx;
		stat.rtt_max = (rtt > stat.rtt_max) ? rtt : stat.rtt_max;
		// stat.rtt_stddev = 
	}
	log_run_ping(&req, &dest_addr, args->options.verbose, init, ip_header, rtt);
	usleep(1000000 - (rtt * 1000));
}

void ping(t_args *args) {
	int serv_fd;
	struct sockaddr_in dest_addr;
	struct timeval timeout;
	struct hostent *he;
	bool init;

	memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.sin_family = AF_INET;
	if (args->host) {
		if (!(he = gethostbyname(args->host)))
			logger("ping: unknown host\n", ERROR, true, 1);
		dest_addr.sin_addr = *((struct in_addr *)he->h_addr_list[0]);
	}
	if ((serv_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1)
		logger("ping: socket: Operation not permitted", ERROR, true, 1);
	timeout.tv_sec = 0;
	timeout.tv_usec = 100000;
	setsockopt(serv_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
	init = true;
	while (true) {
		run(serv_fd, args, dest_addr, init);
		init = false;
	}
}
