/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kichkiro <kichkiro@student.42firenze.it    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 17:05:39 by kichkiro          #+#    #+#             */
/*   Updated: 2025/04/01 17:11:58 by kichkiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

int main(int ac, char **av) {
	(void)ac;
	t_args args;
	char msg[150];

	// memset(&stat, 0, sizeof(stat));
	memset(&args, 0, sizeof(args));
	parser(av, &args);
	if (!args.host_fqdn && !args.host_ip.s_addr) {
		sprintf(msg, "%s%s", OPT_MISSING_HOST, OPT_HELP);
		logger(msg, WARNING, true, 64);
	}
	signal(SIGINT, sigint_handler);
	ping(&args);
	return EXIT_SUCCESS;
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
