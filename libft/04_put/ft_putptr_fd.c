/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putptr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcresce <dcresce@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 15:33:14 by dcresce           #+#    #+#             */
/*   Updated: 2025/11/19 15:33:14 by dcresce          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"

int	ft_putptr_fd(uintptr_t n, int first, int fd)
{
	const char	*hex;
	int			size;

	if (!n)
		return (ft_putstr_fd("(nil)", 1) - 2);
	if (first)
		ft_putstr_fd("0x", 1);
	hex = "0123456789abcdef";
	size = 0;
	if (n >= 16)
		size += ft_putptr_fd(n / 16, 0, fd);
	size += ft_putchar_fd(hex[n % 16], fd);
	return (size);
}
