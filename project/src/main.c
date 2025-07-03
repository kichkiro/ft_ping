/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kichkiro <kichkiro@student.42firenze.it    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 17:05:39 by kichkiro          #+#    #+#             */
/*   Updated: 2025/07/03 22:06:29 by kichkiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

static void free_ptr(void) {
	ft_free((void **)&g.p->socks);
	ft_free((void **)&g.p->req->ip_header);
	ft_free((void **)&g.p->req);
	ft_free((void **)&g.p->resp->ip_header);
	ft_free((void **)&g.p->resp);
	ft_free((void **)&g.p);
}

static void sigint_handler(int sig) {
	if (sig == SIGINT) {
		free_ptr();
		if (g.s.pkt_loss || g.s.pkts_rx || g.s.pkts_tx)
			log_stats();
	}
}

int main(const int ac, char **av) {
	t_args args;

	if (ac == 1)
		logger(LOG_MISS_HOST, WARNING, true, 64);

	if (!(g.p = calloc(1, sizeof(t_ptr))))
		logger("ping: calloc failed", ERROR, true, 1);
	memset(&args, 0, sizeof(args));
	parser(av, &args);
	if (!args.host)
		logger(LOG_MISS_HOST, WARNING, true, 64);
	else
		memcpy(g.s.host, args.host, strlen(args.host)); 	
	signal(SIGINT, sigint_handler);
	ping(&args);
	return EXIT_SUCCESS;
}
