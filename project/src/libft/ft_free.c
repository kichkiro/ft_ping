/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_free.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kichkiro <kichkiro@student.42firenze.it    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 21:36:12 by kichkiro          #+#    #+#             */
/*   Updated: 2025/07/03 21:40:48 by kichkiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

/*!
 * @brief 
	If the ptr is not NULL, it is freed and set to NULL.
 */
void ft_free(void **ptr) {
    if (ptr && *ptr) {
        free(*ptr);
        *ptr = NULL;
    }
}
