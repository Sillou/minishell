/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strsjoin.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcresce <dcresce@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 22:05:00 by dcresce           #+#    #+#             */
/*   Updated: 2025/12/15 22:10:49 by dcresce          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"

char	*ft_strsjoin(char **strs, char sep, int nb)
{
	char	*j_string;
	int		size;
	int		i;
	int		k;

	size = 0;
	if (!strs || !sep)
		return (NULL);
	i = 0;
	while (i < nb)
		size += ft_strlen(strs[i++]);
	j_string = malloc(sizeof(char) * (size + nb));
	if (!j_string)
		return (NULL);
	i = 0;
	k = 0;
	while (i < nb)
	{
		while (*strs[i])
			j_string[k++] = *strs[i]++;
		j_string[k++] = sep;
		i++;
	}
	j_string[--k] = '\0';
	return (j_string);
}
