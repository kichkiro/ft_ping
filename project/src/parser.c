/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kichkiro <kichkiro@student.42firenze.it    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/25 11:33:58 by kichkiro          #+#    #+#             */
/*   Updated: 2025/07/03 20:11:26 by kichkiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

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

static void set_opt_with_value(t_args *args, char **raw, size_t *i) {
	char *opt, *val;

	opt = raw[*i];
	val = raw[*i + 1];
	(*i)++;
	// if (!strcmp(opt, "--linger") || !strcmp(opt, "-W")) {
	// 	args->options.linger.set = true;
	// 	args->options.linger.val = atoi(val);
	// }
	// else if (!strcmp(opt, "--preload") || !strcmp(opt, "-l")) {
	// 	args->options.preload.set = true;
	// 	args->options.preload.val = atoi(val);
	// }
	// else if (!strcmp(opt, "--pattern") || !strcmp(opt, "-p")) {
	// 	args->options.pattern.set = true;
	// 	args->options.pattern.val = atoi(val);
	// }
	if (!strcmp(opt, "--timeout") || !strcmp(opt, "-w")) {
		args->options.timeout.set = true;
		args->options.timeout.val = atoi(val);
	}
	// else if (!strcmp(opt, "--size") || !strcmp(opt, "-s")) {
	// 	args->options.size.set = true;
	// 	args->options.size.val = atoi(val);
	// }
	else if (!strcmp(opt, "--tos") || !strcmp(opt, "-T")) {
		args->options.tos.set = true;
		args->options.tos.val = atoi(val);
	}
	else if (!strcmp(opt, "--ttl")) {
		args->options.ttl.set = true;
		args->options.ttl.val = atoi(val);
	}
	// else if (!strcmp(opt, "--ip-timestamp")) {
	// 	args->options.ip_timestamp.set = true;
	// 	args->options.ip_timestamp.val = atoi(val);
	// }
}

static void info_options(const char *opt) {
	if (!strcmp(opt, "--help") || !strcmp(opt, "-?"))
		logger(LOG_HELP, INFO, true, 0);
	else if (!strcmp(opt, "--version") || !strcmp(opt, "-V"))
		logger(LOG_VERSION, INFO, true, 0);
	else if (!strcmp(opt, "--usage"))
		logger(LOG_USAGE, INFO, true, 0);
}

void parser(char **raw, t_args *args) {
	char logs[140];

	for (size_t i = 1; raw[i]; i++) {
		if (!strcmp(raw[i], "--help") || !strcmp(raw[i], "-?") ||
			!strcmp(raw[i], "--version") || !strcmp(raw[i], "-V") ||
			!strcmp(raw[i], "--usage"))
			info_options(raw[i]);
		else if (!strcmp(raw[i], "--verbose") || !strcmp(raw[i], "-v"))
			args->options.verbose = true;
		else if (!strcmp(raw[i], "--flood") || !strcmp(raw[i], "-f"))
			args->options.flood = true;
		// else if (!strcmp(raw[i], "--numeric") || !strcmp(raw[i], "-n"))
		// 	args->options.numeric = true;
		else if (!strcmp(raw[i], "--ignore-routing") || !strcmp(raw[i], "-r"))
			args->options.ignore_routing = true;
		else if (!strcmp(raw[i], "--linger") || !strcmp(raw[i], "-W") || \
			!strcmp(raw[i], "--preload") || !strcmp(raw[i], "-l") || \
			!strcmp(raw[i], "--pattern") || !strcmp(raw[i], "-p") || \
			!strcmp(raw[i], "--timeout") || !strcmp(raw[i], "-w") || \
			!strcmp(raw[i], "--size") || !strcmp(raw[i], "-s") || \
			!strcmp(raw[i], "--tos") || !strcmp(raw[i], "-T") || \
			!strcmp(raw[i], "--ttl") || !strcmp(raw[i], "--ip-timestamp")) {
			if (!raw[i + 1])
				log_mis_opt_arg(raw[i]);
			else
				set_opt_with_value(args, raw, &i);
		}
		else if (strncmp(raw[i], "-", 1))
			args->host = raw[i];
		else {
			if (!strncmp(raw[i], "--", 2))
				snprintf(logs, sizeof(logs), LOG_INVALID_OPT1 LOG_TRY, raw[i]);
			else
				snprintf(logs, sizeof(logs), LOG_INVALID_OPT2 LOG_TRY, raw[i][1]);
			logger(logs, ERROR, true, 1);
		}
	}
}
