/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kichkiro <kichkiro@student.42firenze.it    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 13:39:41 by kichkiro          #+#    #+#             */
/*   Updated: 2025/04/08 12:51:07 by kichkiro         ###   ########.fr       */
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

static t_packet recv_packet(int serv_fd, struct sockaddr_in *dest_addr) {
	t_packet response_pkt;
	ssize_t bytes;
	char rbuf[1024];
	socklen_t addr_len;
	int ip_header_len;

	memset(&response_pkt, 0, sizeof(response_pkt));
	memset(rbuf, 0, sizeof(rbuf));
	addr_len = sizeof(*dest_addr);
	bytes = recvfrom(serv_fd, rbuf, sizeof(rbuf), 0, (struct sockaddr *)dest_addr, &addr_len);
	response_pkt.ip_header = (struct ip *)rbuf;
	ip_header_len = response_pkt.ip_header->ip_hl * 4;
	response_pkt.icmp_header = (struct icmp *)(rbuf + ip_header_len);
	if (bytes <= 0 || response_pkt.icmp_header->icmp_type == 3 
		|| response_pkt.icmp_header->icmp_type == 11)
		stat.pkt_loss++;
	else
		stat.pkts_rx++;
	return response_pkt;
}

static void send_packet(int serv_fd, struct sockaddr_in *dest_addr, t_icmp_packet *request) {
	if (sendto(serv_fd, request, sizeof(*request), 0,
			   (struct sockaddr *)dest_addr, sizeof(*dest_addr)) <= 0)
		logger("ping: send failed", ERROR, true, 1);
	else
		stat.pkts_tx++;
}

static void set_packet(t_icmp_packet *request, bool init) {
	struct timeval ts;
	static char data[40];

	if (init) {
		for (int i = 0; i < 40; i++)
			data[i] = (unsigned char)i;
		memset(request, 0, sizeof(*request));
		request->header.type = ICMP_ECHO;
		request->header.code = 0;
		request->header.un.echo.id = getpid();
		request->header.un.echo.sequence = 0;
	}
	else
		request->header.un.echo.sequence++;
	memset(&ts, 0, sizeof(ts));
	gettimeofday(&ts, 0);
	memset(request->payload, 0, 64);
	memcpy(request->payload, &ts, sizeof(ts));
	memcpy(request->payload + sizeof(ts), &data, sizeof(data));
	request->header.checksum = 0;
	request->header.checksum = checksum(request, sizeof(*request));
}

static void run(int serv_fd, t_args *args, struct sockaddr_in dest_addr, bool init) {
	t_icmp_packet request;
	t_packet response;
	struct timeval ts_start, ts_end;
	double rtt;

	if (init)
		log_run_ping_init(&request, &dest_addr, args->options.verbose);
	set_packet(&request, init);
	gettimeofday(&ts_start, 0);
	send_packet(serv_fd, &dest_addr, &request);
	response = recv_packet(serv_fd, &dest_addr);
	gettimeofday(&ts_end, 0);
	rtt = ((ts_end.tv_sec - ts_start.tv_sec) * 1000) + \
		((ts_end.tv_usec - ts_start.tv_usec) / 1000.0);
	if (response.ip_header) {
		stat.rtt_min = (rtt < stat.rtt_min || !stat.rtt_min) ? rtt : stat.rtt_min;
		stat.rtt_avg = (stat.rtt_avg * (stat.pkts_rx - 1) + rtt) / stat.pkts_rx;
		stat.rtt_max = (rtt > stat.rtt_max) ? rtt : stat.rtt_max;
		stat.rtt_m2 = (stat.rtt_m2 * (stat.pkts_rx - 1) + rtt * rtt) / stat.pkts_rx;
		stat.rtt_stddev = sqrt(stat.rtt_m2 - stat.rtt_avg * stat.rtt_avg);
	}
	log_run_ping(&request, &dest_addr, response, rtt,args->options.verbose);
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
	// int ttl = 10;
	// setsockopt(serv_fd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));
	timeout.tv_sec = 0;
	timeout.tv_usec = 100000;
	setsockopt(serv_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
	init = true;
	while (true) {
		run(serv_fd, args, dest_addr, init);
		init = false;
	}
	close(serv_fd);
}
