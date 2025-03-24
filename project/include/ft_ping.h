/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kichkiro <kichkiro@student.42firenze.it    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 17:00:59 by kichkiro          #+#    #+#             */
/*   Updated: 2025/03/24 17:17:56 by kichkiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PING
# define FT_PING

// Libraries ------------------------------------------------------------------>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Colors --------------------------------------------------------------------->
#define NO_COLOR    "\033[0m"
#define BLUE        "\033[34m"
#define YELLOW      "\033[33m"
#define RED         "\033[31m"

// Log - Levels --------------------------------------------------------------->
#define DEBUG   1
#define INFO    2
#define WARNING 3
#define ERROR   4

// Main ----------------------------------------------------------------------->

// Utils  --------------------------------------------------------------------->

void logger(char *msg, int level, bool to_exit, int exit_code);


#endif
