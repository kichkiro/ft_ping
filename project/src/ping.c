/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kichkiro <kichkiro@student.42firenze.it    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 13:39:41 by kichkiro          #+#    #+#             */
/*   Updated: 2025/04/01 17:26:05 by kichkiro         ###   ########.fr       */
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

	addr_len = sizeof(*dest_addr);
	if ((*bytes = recvfrom(serv_fd, rbuf, sizeof(rbuf), 0, 
		(struct sockaddr *)dest_addr, &addr_len)) <= 0) {
		stat.pkt_loss++;
	}
	else
		stat.pkts_rx++;
	ip_header = (struct ip *)rbuf;
	return ip_header;
}

static void send_packet(int serv_fd, struct sockaddr_in *dest_addr, t_icmp_packet *req) {
	if (sendto(serv_fd, req, sizeof(*req), 0,
			   (struct sockaddr *)dest_addr, sizeof(*dest_addr)) <= 0)
		logger("ping: send failed", ERROR, true, 1);
	else
		stat.pkts_tx++;
}

static void update_packet(t_icmp_packet *req) {
	struct timeval ts;

	req->header.un.echo.sequence++;
	req->header.checksum = 0;
	gettimeofday(&ts, 0);
	memcpy(req->payload, &ts, sizeof(ts));
	req->header.checksum = checksum(req, sizeof(*req));
}

static void set_packet(t_icmp_packet *req) {
	struct timeval ts;

	memset(req, 0, sizeof(*req));
	req->header.type = ICMP_ECHO;
	req->header.code = 0;
	req->header.un.echo.id = getpid();
	req->header.un.echo.sequence = 0;
	gettimeofday(&ts, 0);
	memcpy(req->payload, &ts, sizeof(ts));
	req->header.checksum = checksum(req, sizeof(*req));
}

static void set_dest_addr(t_args *args, struct sockaddr_in *addr) {
	struct hostent *he;

	memset(addr, 0, sizeof(*addr));
	addr->sin_family = AF_INET;
	if (args->host_fqdn) {
		if (!(he = gethostbyname(args->host_fqdn)))
			logger("ping: unknown host\n", ERROR, true, 1);
		addr->sin_addr = *((struct in_addr *)he->h_addr_list[0]);
	}
	else
		addr->sin_addr.s_addr = args->host_ip.s_addr;
}

static void execute(int serv_fd, t_icmp_packet req, struct sockaddr_in dest_addr) {
	struct ip *ip_header;
	struct timeval ts_start, ts_end;
	double rtt;
	ssize_t bytes_recv;
	char logs[100];
	
	gettimeofday(&ts_start, 0);
	send_packet(serv_fd, &dest_addr, &req);
	ip_header = recv_packet(serv_fd, &dest_addr, &bytes_recv);
	gettimeofday(&ts_end, 0);
	rtt = ((ts_end.tv_sec - ts_start.tv_sec) * 1000) + ((ts_end.tv_usec - ts_start.tv_usec) / 1000.0);
	sprintf(logs, "%d bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n",
		ntohs(ip_header->ip_len) - ip_header->ip_hl * 4,
		inet_ntoa(dest_addr.sin_addr), req.header.un.echo.sequence,
		ip_header->ip_ttl, rtt
	);
	logger(logs, INFO, false, 0);
	usleep(1000000); // SISTEMARE QUI
	update_packet(&req);
}

void ping(t_args *args) {
	int serv_fd;
	t_icmp_packet req;
	struct sockaddr_in dest_addr;
	struct timeval timeout;
	char logs[100];

	
	set_dest_addr(args, &dest_addr);
	if ((serv_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1)
		logger("ping: socket: Operation not permitted", ERROR, true, 1);
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;
	setsockopt(serv_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
	set_packet(&req);
	if (args->options.verbose)
		sprintf(logs, "PING %s (%s): %ld data bytes, id 0x0%x = %d\n",
			inet_ntoa(dest_addr.sin_addr), inet_ntoa(dest_addr.sin_addr),
			sizeof(req.payload), req.header.un.echo.id, req.header.un.echo.id
		);
	else
		sprintf(logs, "PING %s (%s): %ld data bytes\n",
			inet_ntoa(dest_addr.sin_addr), inet_ntoa(dest_addr.sin_addr),
			sizeof(req.payload)
		);
	logger(logs, INFO, false, 0);
	while (true) {
		execute(serv_fd, req, dest_addr);
	}
	close(serv_fd);
}
