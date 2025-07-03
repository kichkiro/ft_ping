/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kichkiro <kichkiro@student.42firenze.it    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/02 15:25:09 by kichkiro          #+#    #+#             */
/*   Updated: 2025/07/03 20:07:18 by kichkiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

t_sock *get_socket(const t_args *args) {
    t_sock *socks;
    struct timeval to;
    int on;

	on = 1;
    to.tv_sec = 0;
    to.tv_usec = args->options.flood ? 0 : 100000;  
	if (!(socks = calloc(1, sizeof(t_sock))))
		logger("ping: calloc failed", ERROR, true, 1);
    if ((socks->send_fd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) == -1)
		logger("ping: socket failed\n", ERROR, true, 1);
	if ((socks->recv_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1)
		logger("ping: socket failed\n", ERROR, true, 1);
    if (setsockopt(socks->recv_fd, SOL_SOCKET, SO_RCVTIMEO, &to, sizeof(to)) == -1)
        logger("ping: setsockopt failed\n", ERROR, true, 1);
    if (setsockopt(socks->send_fd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)) == -1)
        logger("ping: setsockopt failed\n", ERROR, true, 1);
    if (args->options.ignore_routing) {
		if (setsockopt(socks->send_fd, SOL_SOCKET, SO_DONTROUTE, &on, sizeof(on)) == -1)
			logger("ping: setsockopt failed\n", ERROR, true, 1);
	}
	return socks;
}
