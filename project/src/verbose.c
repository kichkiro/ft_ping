/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   verbose.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kichkiro <kichkiro@student.42firenze.it    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 13:12:37 by kichkiro          #+#    #+#             */
/*   Updated: 2025/07/03 20:32:43 by kichkiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

static char *get_ip_hdr_hex(const t_pkt *resp) {
    char hex_dump[200] = {0};
    uint8_t *raw_hdr;
    size_t pos;

    raw_hdr = (uint8_t *)resp->ip_header;
	pos = 0;
    pos += snprintf(hex_dump + pos, sizeof(hex_dump) - pos, "IP Hdr Dump:\n ");
    for (int i = 28; i < 48; i += 2) {
        pos += snprintf(hex_dump + pos, sizeof(hex_dump) - pos, "%02x%02x ", 
            raw_hdr[i], raw_hdr[i + 1]);
    }
    pos += snprintf(hex_dump + pos, sizeof(hex_dump) - pos, "\n");
    return ft_strf("%s", hex_dump);
}

static char *get_ip_hdr(const t_pkt *resp) {
    char *logs;
    uint8_t *hdr = (uint8_t *)resp->ip_header;
    uint8_t version_ihl = hdr[0];
    uint8_t version = version_ihl >> 4;
    uint8_t ihl = version_ihl & 0x0F;
    uint8_t tos = hdr[1];
    uint16_t total_length = (hdr[2] << 8) | hdr[3];
    uint16_t identification = (hdr[4] << 8) | hdr[5];
    uint16_t flags_frag = (hdr[6] << 8) | hdr[7];
    uint8_t ttl = hdr[8];
    uint8_t protocol = hdr[9];
    uint16_t checksum = (hdr[10] << 8) | hdr[11];
    uint8_t flags = (flags_frag >> 13) & 0x7;
    uint16_t offset = flags_frag & 0x1FFF;

    char src_ip_str[16];
    char dst_ip_str[16];
    snprintf(src_ip_str, sizeof(src_ip_str), "%u.%u.%u.%u", hdr[12], hdr[13], hdr[14], hdr[15]);
    snprintf(dst_ip_str, sizeof(dst_ip_str), "%u.%u.%u.%u", hdr[16], hdr[17], hdr[18], hdr[19]);
    logs = ft_strf(
        LOG_IP_HDR,
        version,
        ihl,
        tos,
        total_length,
        identification,
        flags,
        offset,
        ttl,
        protocol,
        checksum,
        src_ip_str,
        dst_ip_str
    );
    return logs;
}

static char *get_icmp_hdr(const t_pkt *req) {
    int pl_sz;
    struct icmphdr hdr;

    hdr = req->icmp_pkt->header;
    pl_sz = ntohs(req->ip_header->ip_len) - (req->ip_header->ip_hl * 4);
    return ft_strf(LOG_ICMP_HDR, hdr.type, hdr.code, pl_sz, 
        ntohs(hdr.un.echo.id), ntohs(hdr.un.echo.sequence)
    );
}

char *get_verbose_body(const t_pkt *resp, const t_pkt *req) {
    char *ip_hdr_hex;
    char *ip_hdr;
    char *icmp_hdr;
    char *logs;

    ip_hdr_hex = get_ip_hdr_hex(resp);
    ip_hdr = get_ip_hdr(resp);
    icmp_hdr = get_icmp_hdr(req);
    if (!ip_hdr_hex || !ip_hdr || !icmp_hdr) {
        free(ip_hdr_hex);
        free(ip_hdr);
        free(icmp_hdr);
        return NULL;
    }
    logs = ft_strf("%s%s%s", ip_hdr_hex, ip_hdr, icmp_hdr);
    free(ip_hdr_hex);
    free(ip_hdr);
    free(icmp_hdr);
    return logs;
}

void get_verbose_hdr(const struct in_addr *dst_ip, const t_pkt *req) {
    char *logs;
    uint16_t pid;
    size_t pl_sz;
    char ip_buf[INET_ADDRSTRLEN];

    if (!req || !req->icmp_pkt)
        logger("get_verbose_hdr(): invalid packet", ERROR, true, 1);
    inet_ntop(AF_INET, &(dst_ip), ip_buf, sizeof(ip_buf));
    pid = req->icmp_pkt->header.un.echo.id;
    pl_sz = sizeof(req->icmp_pkt->payload);
    if (!(logs = ft_strf(LOG_PING_HDR_VERB, g.s.host, ip_buf, pl_sz, pid, pid)))
        logger("get_verbose_hdr(): ft_strf allocation failed", ERROR, true, 1);
    logger(logs, INFO, false, 0);
    free(logs);
}
