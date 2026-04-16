/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: credomgiu <credomgiu@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 16:51:27 by dcresce           #+#    #+#             */
/*   Updated: 2025/10/10 22:59:57 by credomgiu        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	unsigned char	*dest_tmp;
	unsigned char	*src_tmp;

	if (!dest && !src)
		return (NULL);
	dest_tmp = (unsigned char *)dest;
	src_tmp = (unsigned char *)src;
	while (n--)
		*dest_tmp++ = *src_tmp++;
	return (dest);
}
