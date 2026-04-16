/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcresce <dcresce@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/01 11:48:25 by dcresce           #+#    #+#             */
/*   Updated: 2026/02/01 11:49:04 by dcresce          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"

static int	flag_handler(const char flag, va_list *args);

int	ft_printf(const char *argc, ...)
{
	va_list		args;
	int			size;

	va_start(args, argc);
	size = 0;
	while (*argc)
	{
		if (*argc != '%')
		{
			ft_putchar_fd((char)*argc, 1);
			size++;
		}
		else
		{
			argc++;
			size += flag_handler(*argc, &args);
		}
		argc++;
	}
	va_end(args);
	return (size);
}

static int	flag_handler(const char flag, va_list *args)
{
	if (flag == 'c')
		return (ft_putchar_fd(va_arg(*args, int), 1));
	if (flag == 's')
		return (ft_putstr_fd(va_arg(*args, char *), 1));
	if (flag == 'p')
		return (ft_putptr_fd((uintptr_t)va_arg(*args, void *), 1, 1) + 2);
	if (flag == 'd' || flag == 'i')
		return (ft_putnbr_fd(va_arg(*args, int), 1));
	if (flag == 'u')
		return (ft_putunbr_fd(va_arg(*args, int), 1));
	if (flag == 'x')
		return (ft_putnbr_hex_fd(va_arg(*args, int), 0, 1));
	if (flag == 'X')
		return (ft_putnbr_hex_fd(va_arg(*args, int), 1, 1));
	if (flag == '%')
		return (ft_putchar_fd('%', 1));
	return (0);
}
