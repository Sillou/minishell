/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putstr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcresce <dcresce@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 15:06:15 by dcresce           #+#    #+#             */
/*   Updated: 2025/11/18 15:06:15 by dcresce          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"

int	ft_putstr_fd(char *s, int fd)
{
	int	size;

	if (!s)
		return (ft_putstr_fd("(null)", 1));
	size = ft_strlen(s);
	while (*s)
		write(fd, s++, 1);
	return (size);
}
