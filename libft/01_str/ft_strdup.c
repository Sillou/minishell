/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcresce <dcresce@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 15:48:48 by dcresce           #+#    #+#             */
/*   Updated: 2025/11/19 15:48:48 by dcresce          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"

char	*ft_strdup(const char *s)
{
	char	*output;
	int		i;

	output = (char *)malloc(sizeof(char) * (ft_strlen(s) + 1));
	if (!output)
		return (NULL);
	i = 0;
	while (*s)
		output[i++] = *s++;
	output[i] = '\0';
	return (output);
}
