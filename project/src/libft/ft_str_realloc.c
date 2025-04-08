/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_str_realloc.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kichkiro <kichkiro@student.42firenze.it    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 14:20:51 by kichkiro          #+#    #+#             */
/*   Updated: 2025/04/08 15:46:08 by kichkiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

size_t ft_str_realloc(char **p, size_t old_size, size_t add_size) {
	char *new_ptr;

	if (!(new_ptr = (char *)realloc(*p, (old_size + add_size) * sizeof(char))))
		logger("ping: calloc: memory allocation failed", ERROR, true, 1);
	*p = new_ptr;
	return old_size + add_size;
}
