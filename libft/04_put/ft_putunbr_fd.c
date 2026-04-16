/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putunbr_fd.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcresce <dcresce@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 15:40:59 by dcresce           #+#    #+#             */
/*   Updated: 2025/11/19 15:40:59 by dcresce          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"

int	ft_putunbr_fd(unsigned int n, int fd)
{
	int		size;

	size = 0;
	if (n > 9)
	{
		size += ft_putunbr_fd(n / 10, fd);
		size += ft_putchar_fd((n % 10) + '0', fd);
	}
	else
		size += ft_putchar_fd(n + '0', fd);
	return (size);
}
