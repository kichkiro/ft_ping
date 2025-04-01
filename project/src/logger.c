/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kichkiro <kichkiro@student.42firenze.it    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 17:05:01 by kichkiro          #+#    #+#             */
/*   Updated: 2025/03/31 12:00:55 by kichkiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

void logger(char *msg, int level, bool to_exit, int exit_code) {
	char new_msg[500];
	int fd;

	if (msg && *msg) {
		if (level == DEBUG) {
			fd = 1;
			sprintf(new_msg, "%s%s%s", NO_COLOR, msg, NO_COLOR);
		}
		else if (level == INFO) {
			fd = 1;
			sprintf(new_msg, "%s%s%s", BLUE, msg, BLUE);
		}
		else if (level == WARNING) {
			fd = 2;
			sprintf(new_msg, "%s%s%s", YELLOW, msg, YELLOW);
		}
		else if (level == ERROR) {
			fd = 2;
			sprintf(new_msg, "%s%s%s", RED, msg, RED);
		}
		write(fd, new_msg, strlen(new_msg));
	}
	if (to_exit)
		exit(exit_code);
}
