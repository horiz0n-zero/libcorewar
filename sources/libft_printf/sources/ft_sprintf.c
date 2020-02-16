/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_sprintf.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: afeuerst <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/17 10:09:33 by afeuerst          #+#    #+#             */
/*   Updated: 2020/02/14 10:15:57 by afeuerst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft_printf_internal.h"

static int						ft_sprintf_function(const char *const formatted, const size_t length);

static struct s_printformat		printformat =
{
	.format = NULL,
	.args = NULL,
	.percents = NULL,
	.length = 0,
	.r1 = 0,
	.r2 = 0,
	.function = ft_sprintf_function
};

static char						*g_dst = NULL;

static int						ft_sprintf_function(const char *const formatted, const size_t length)
{
	size_t						index;

	index = 0;
	while (index < length)
	{
		g_dst[index] = formatted[index];
		++index;
	}
	g_dst[index] = 0;
	return (int)length;
}

int								ft_sprintf(char *const dst, const char *const format, ...)
{
	va_list						args;

	g_dst = dst;
	va_start(args, format);
	printformat.length = 0;
	printformat.format = format;
	printformat.args = &args;
	ft_printf_core(&printformat, format, &printformat.percents);
	va_end(args);
	return (printformat.ret);
}





