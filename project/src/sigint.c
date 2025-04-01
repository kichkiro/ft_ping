/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sigint.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kichkiro <kichkiro@student.42firenze.it    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 15:32:05 by kichkiro          #+#    #+#             */
/*   Updated: 2025/04/01 17:08:38 by kichkiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

extern t_statistics stat;

void sigint_handler(int sig) {
	char logs[200];
	// t_statistics stat;

	// stat = statistics_handler(GET);
	if (sig == SIGINT) {
		sprintf(logs,
			"--- %s ping statistics ---\n"
			"%d packets transmitted, %d packets received, %d packet loss\n"
			"round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n",
			stat.host, stat.pkts_tx, stat.pkts_rx, stat.pkt_loss, stat.rtt_min, 
			stat.rtt_avg, stat.rtt_max, stat.rtt_stddev
		);
		logger(logs, INFO, true, EXIT_SUCCESS);
	}
}

// void statistics_handler(char action) {
// 	// static t_statistics stat;

// 	if (action == GET) {
// 		// return stat;
// 	}
// 	// else if (action == SET) {
// 	// 	stat.host = 0;
// 	// 	stat.pkts_tx = 0;
// 	// 	stat.pkts_rx = 0;
// 	// 	stat.pkt_loss = 0;
// 	// 	stat.rtt_min = 0;
// 	// 	stat.rtt_avg = 0;
// 	// 	stat.rtt_max = 0;
// 	// 	stat.rtt_stddev = 0;
// 	// }
// 	// return stat;
// }
