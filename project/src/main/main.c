/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kichkiro <kichkiro@student.42firenze.it    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 17:05:39 by kichkiro          #+#    #+#             */
/*   Updated: 2025/03/24 17:25:27 by kichkiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

int main(int ac, char **av) {
	(void)av;

	if (ac == 1)
		logger("ping: missing host operand\nTry 'ping --help' or 'ping --usage' for more information.", WARNING, true, 64);


		
	return 0;
}
