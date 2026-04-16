/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr_hex_fd.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcresce <dcresce@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 15:40:51 by dcresce           #+#    #+#             */
/*   Updated: 2025/11/19 15:40:51 by dcresce          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"

int	ft_putnbr_hex_fd(unsigned int n, int caps, int fd)
{
	const char	*hex;
	int			size;

	if (!caps)
		hex = "0123456789abcdef";
	else
		hex = "0123456789ABCDEF";
	size = 0;
	if (n >= 16)
		size += ft_putnbr_hex_fd(n / 16, caps, fd);
	size += ft_putchar_fd(hex[n % 16], fd);
	return (size);
}
