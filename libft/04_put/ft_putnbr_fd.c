/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcresce <dcresce@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 15:20:13 by dcresce           #+#    #+#             */
/*   Updated: 2025/11/18 15:23:25 by dcresce          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"

int	ft_putnbr_fd(int n, int fd)
{
	long	nb;
	int		size;

	nb = n;
	size = 0;
	if (nb < 0)
	{
		size += write(fd, "-", 1);
		nb = -nb;
	}
	if (nb >= 10)
	{
		size += ft_putnbr_fd(nb / 10, fd);
		size += ft_putchar_fd((nb % 10) + '0', fd);
	}
	else
		size += ft_putchar_fd(nb + '0', fd);
	return (size);
}
