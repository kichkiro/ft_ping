/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   packets.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kichkiro <kichkiro@student.42firenze.it    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/02 15:33:33 by kichkiro          #+#    #+#             */
/*   Updated: 2025/07/03 21:57:43 by kichkiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

void recv_packet(const int fd, const struct sockaddr_in *dst_ip, t_pkt *resp) {
    uint8_t *rbuf;
    ssize_t bytes;
    socklen_t addr_sz = sizeof(*dst_ip);
    struct ip *ip_hdr;
    int hdr_sz;

    if (!(rbuf = calloc(1, 1024)))
        logger("ping: calloc failed", ERROR, true, 1);
    bytes = recvfrom(fd, rbuf, 1024, 0, (struct sockaddr *)dst_ip, &addr_sz);
    if (bytes <= 0) {
        free(rbuf);
        g.s.pkt_loss++;
        return;
    }
    ip_hdr = (struct ip *)rbuf;
    hdr_sz = ip_hdr->ip_hl * 4;
    if (bytes < (ssize_t)sizeof(struct ip) ||
        bytes < (ssize_t)(hdr_sz + sizeof(struct icmphdr))) {
        free(rbuf);
        g.s.pkt_loss++;
        return;
    }
    resp->ip_header = (struct ip *)rbuf;
    resp->icmp_pkt = (t_icmp_pkt *)(rbuf + hdr_sz);
    g.p->resp->ip_header = resp->ip_header;
    g.p->resp->icmp_pkt = resp->icmp_pkt;
    if (resp->icmp_pkt->header.type == ICMP_DEST_UNREACH ||
        resp->icmp_pkt->header.type == ICMP_TIME_EXCEEDED)
        g.s.pkt_loss++;
    else
        g.s.pkts_rx++;
}

void send_packet(const int fd, const struct sockaddr_in *dst_ip, t_pkt *req) {
    size_t packet_size = sizeof(struct ip) + sizeof(t_icmp_pkt);
    char buffer[sizeof(struct ip) + sizeof(t_icmp_pkt)];

    memcpy(buffer, req->ip_header, sizeof(struct ip));
    memcpy(buffer + sizeof(struct ip), req->icmp_pkt, sizeof(t_icmp_pkt));
    ssize_t sent = sendto(
        fd,
        buffer,
        packet_size,
        0,
        (struct sockaddr *)dst_ip,
        sizeof(*dst_ip)
    );
    if (sent <= 0)
        logger("ping: sending packet: Network is unreachable", ERROR, true, 1);
    else
        g.s.pkts_tx++;
}

static unsigned short get_checksum(const void *data, int data_sz) {
	const unsigned short *buf;
	unsigned int sum;

	buf = data;
	sum = 0;
	while (data_sz > 1) {
		sum += *buf++;
		data_sz -= 2;
	}
	if (data_sz == 1)
		sum += *((uint8_t *)buf);
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	return (unsigned short)~sum;
}

static void set_icmp(const t_pkt *req, const bool init) {
    static char data[40];
    struct timeval ts;
    static uint16_t seq = 0;

    // ICMP Header ------------------------------------------------------------>
    if (init) {
        for (int i = 0; i < 40; i++)
            data[i] = (unsigned char)i;
    }
    req->icmp_pkt->header.type = ICMP_ECHO;
    req->icmp_pkt->header.code = 0;
    req->icmp_pkt->header.un.echo.id = htons(getpid() & 0xFFFF);
    req->icmp_pkt->header.un.echo.sequence = htons(seq++);

    // ICMP Payload ----------------------------------------------------------->
    gettimeofday(&ts, 0);
    memset(req->icmp_pkt->payload, 0, sizeof(req->icmp_pkt->payload));
    memcpy(req->icmp_pkt->payload, &ts, sizeof(ts));
    memcpy(req->icmp_pkt->payload + sizeof(ts), &data, sizeof(data));

    // ICMP Checksum ---------------------------------------------------------->
    req->icmp_pkt->header.checksum = 0;
    req->icmp_pkt->header.checksum = get_checksum(
        (uint16_t *)req->icmp_pkt,
        sizeof(struct icmphdr) + sizeof(req->icmp_pkt->payload)
    );
}

static void set_ip_hdr(
    t_pkt *req, 
    const struct in_addr dst_ip, 
    const t_opt opt
) {
    req->ip_header->ip_hl = 5;
    req->ip_header->ip_v = 4;
    req->ip_header->ip_tos = opt.tos.set ? opt.tos.val : 0;
    req->ip_header->ip_len = htons(sizeof(struct ip) + sizeof(t_icmp_pkt));
    req->ip_header->ip_id = htons(getpid() & 0xFFFF);
    req->ip_header->ip_off = htons(IP_DF);
    req->ip_header->ip_ttl = opt.ttl.set ? opt.ttl.val : 64;
    req->ip_header->ip_p = IPPROTO_ICMP;
    req->ip_header->ip_sum = 0;
    req->ip_header->ip_src.s_addr = INADDR_ANY;
    req->ip_header->ip_dst = dst_ip;
}

void set_req_packet(
    t_pkt *req, 
    const struct in_addr dst_ip, 
    const bool init, 
    const t_opt opt
) {
    if (!(req->ip_header = calloc(1, sizeof(struct ip))))
        logger("ping: calloc failed", ERROR, true, 1);
    if (!(req->icmp_pkt = calloc(1, sizeof(t_icmp_pkt))))
        logger("ping: calloc failed", ERROR, true, 1);
    set_ip_hdr(req, dst_ip, opt);
    g.p->req->ip_header = req->ip_header;
    set_icmp(req, init);
    g.p->req->icmp_pkt = req->icmp_pkt;
}
