/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strf.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kichkiro <kichkiro@student.42firenze.it    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 14:34:34 by kichkiro          #+#    #+#             */
/*   Updated: 2025/06/30 14:35:57 by kichkiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * ft_strf - Allocate a formatted string dynamically.
 *
 * This function builds a formatted string using a printf-like format and
 * variable arguments, allocating exactly enough memory to hold the result,
 * including the null terminator. The caller is responsible for freeing the
 * returned string using free().
 *
 * Example usage:
 *     char *s = ft_strf("Hello %s, value = %d", "Alice", 42);
 *     if (s) {
 *         puts(s);
 *         free(s);
 *     }
 *
 * @param fmt: The format string (same syntax as printf).
 * @param ...: Variable arguments corresponding to the format specifiers.
 *
 * @return A pointer to the newly allocated string, or NULL on error.
 */
char *ft_strf(const char *fmt, ...) {
    va_list ap;
    va_list ap2;
    int needed;
    char *buf;

    if (!fmt)
        return NULL;
    va_start(ap, fmt);
    va_copy(ap2, ap);
    needed = vsnprintf(NULL, 0, fmt, ap);
    va_end(ap);
    if (needed < 0)
        return NULL;
    buf = calloc((size_t)needed + 1, 1);
    if (!buf) {
        va_end(ap2);
        return NULL;
    }
    vsnprintf(buf, (size_t)needed + 1, fmt, ap2);
    va_end(ap2);
    return buf;
}
