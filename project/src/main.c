/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kichkiro <kichkiro@student.42firenze.it    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 17:05:39 by kichkiro          #+#    #+#             */
/*   Updated: 2025/04/07 11:34:09 by kichkiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

static void sigint_handler(int sig) {
	if (sig == SIGINT) {
		if (stat.pkt_loss || stat.pkts_rx || stat.pkts_tx)
			log_statistics();
	}
}

int main(int ac, char **av) {
	t_args args;

	if (ac == 1)
		log_missing_host();
	memset(&args, 0, sizeof(args));
	parser(av, &args);
	if (!args.host)
		log_missing_host();
	else
		memcpy(stat.host, args.host, strlen(args.host)); 	
	signal(SIGINT, sigint_handler);
	ping(&args);
	return EXIT_SUCCESS;
}
