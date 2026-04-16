/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcresce <dcresce@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/12 00:17:27 by dcresce           #+#    #+#             */
/*   Updated: 2025/10/12 00:24:36 by dcresce          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	unsigned char	*s_tmp;

	s_tmp = (unsigned char *)s;
	if (!n)
		return (NULL);
	while (n)
	{
		if (*s_tmp == (unsigned char)c)
			return (s_tmp);
		s_tmp++;
		n--;
	}
	return (NULL);
}
