/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kichkiro <kichkiro@student.42firenze.it    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 13:39:41 by kichkiro          #+#    #+#             */
/*   Updated: 2025/03/26 17:12:21 by kichkiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

static unsigned short checksum(void *b, int len) {    
	unsigned short result;
    unsigned short *buf;
    unsigned int sum;

	buf = b;
    sum = 0;
    while (len > 1) {
        sum += *buf++;
        len -= 2;
    }    
    if (len == 1)
        sum += *(unsigned char *)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

static struct ip *recv_packet(int serv_fd, struct sockaddr_in *dest_addr, ssize_t *bytes) {
	struct ip *ip_header;
	char recv_buffer[1024];
	socklen_t addr_len;

	addr_len = sizeof(*dest_addr);
	if ((*bytes = recvfrom(serv_fd, recv_buffer, sizeof(recv_buffer), 0, 
		(struct sockaddr *)dest_addr, &addr_len)) <= 0) {
		logger("ping: receive failed", ERROR, true, 1);
	}
	ip_header = (struct ip *)recv_buffer;
	return ip_header;
}

static void send_packet(int serv_fd, struct sockaddr_in *dest_addr, struct icmphdr *icmp_header) {
	if (sendto(serv_fd, icmp_header, sizeof(*icmp_header), 0, 
		(struct sockaddr *)dest_addr, sizeof(*dest_addr)) <= 0) {
		logger("ping: send failed", ERROR, true, 1);
	}
}

static void update_packet(struct icmphdr *icmp_header) {
	icmp_header->un.echo.sequence++;
	icmp_header->checksum = 0;
	icmp_header->checksum = checksum(icmp_header, sizeof(*icmp_header));
}

static void set_packet(struct icmphdr *icmp_header) {
	memset(icmp_header, 0, sizeof(*icmp_header));
    icmp_header->type = ICMP_ECHO;
    icmp_header->code = 0;
    icmp_header->un.echo.id = getpid();
	icmp_header->un.echo.sequence = 0;
	icmp_header->checksum = checksum(icmp_header, sizeof(*icmp_header));
}

static void set_dest_addr(t_args *args, struct sockaddr_in *addr) {
    struct hostent *he;
	
	memset(addr, 0, sizeof(*addr));
	addr->sin_family = AF_INET;
	if (args->host_fqdn) {
		if ((he = gethostbyname(args->host_fqdn)) == NULL)
			logger("ping: unknown host\n", ERROR, true, 1);
		addr->sin_addr = *((struct in_addr *)he->h_addr_list[0]);
	} else
		addr->sin_addr.s_addr = args->host_ip.s_addr;
}

void ping(t_args *args) {
    int serv_fd;
    struct sockaddr_in dest_addr;
    struct icmphdr icmp_header;
	struct ip *ip_header;
    struct timespec start, end;
    double rtt;
	ssize_t bytes_recv;
	char logs[100];

    set_dest_addr(args, &dest_addr);
    if ((serv_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1) {
        logger("ping: socket: Operation not permitted", ERROR, true, 1);
	}
	set_packet(&icmp_header);
	sprintf(logs, "PING %s (%s): %ld data bytes\n", 
		inet_ntoa(dest_addr.sin_addr), inet_ntoa(dest_addr.sin_addr), 
		sizeof(icmp_header));
	logger(logs, INFO, false, 0);
    while (true) {
        clock_gettime(CLOCK_MONOTONIC, &start);
        send_packet(serv_fd, &dest_addr, &icmp_header);
		ip_header = recv_packet(serv_fd, &dest_addr, &bytes_recv);
        clock_gettime(CLOCK_MONOTONIC, &end);
		rtt = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_nsec - start.tv_nsec) / 1000000.0;
        sprintf(logs, "%ld bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n", 
			bytes_recv, inet_ntoa(dest_addr.sin_addr), 
			icmp_header.un.echo.sequence, ip_header->ip_ttl, rtt);
		logger(logs, INFO, false, 0);
		sleep(1);
		update_packet(&icmp_header);
	}
    close(serv_fd);
}
