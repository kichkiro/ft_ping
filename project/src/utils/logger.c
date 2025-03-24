/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kichkiro <kichkiro@student.42firenze.it    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 17:05:01 by kichkiro          #+#    #+#             */
/*   Updated: 2025/03/24 17:19:21 by kichkiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

void logger(char *msg, int level, bool to_exit, int exit_code) {
	if (msg && *msg) {
		if (level == WARNING) {
			write(2, YELLOW, strlen(YELLOW));
			write(2, msg, strlen(msg));
			write(2, NO_COLOR, strlen(NO_COLOR));
		}
		else if (level == ERROR)
			write(2, msg, strlen(msg));
	}
	if (to_exit)
		exit(exit_code);
}
