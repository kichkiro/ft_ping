/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kichkiro <kichkiro@student.42firenze.it    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 17:05:39 by kichkiro          #+#    #+#             */
/*   Updated: 2025/03/25 16:52:01 by kichkiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

int main(int ac, char **av) {
	(void)ac;
	t_args args;
	char msg[150];

	memset(&args, 0, sizeof(args));
	parser(av, &args);
	if (!args.host_fqdn && !args.host_ip) {
		sprintf(msg, "%s%s", OPT_MISSING_HOST, OPT_HELP);
		logger(msg, WARNING, true, 64);
	}

	return 0;
}


/**
OPTIONS:
	--usage
	-? --help
	-V --version
	-v --verbose

	-f --flood
	-l --preload=NUMBER
	-n --numeric
	-w --timeout=N
	-W --linger=N
	-p --pattern=PATTERN
	-r --ignore-routing
	-s --size=NUMBER
	-T --tos=NUM
	--ttl=N
	--ip-timestamp=FLAG
*/
