/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcresce <dcresce@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/10 13:22:19 by dcresce           #+#    #+#             */
/*   Updated: 2025/10/10 13:39:32 by dcresce          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"

static int	ft_isspace(char c);

int	ft_atoi(const char *nptr)
{
	int	result;
	int	minus;

	result = 0;
	minus = 1;
	while (ft_isspace(*nptr))
		nptr++;
	if ((*nptr == '+') || (*nptr == '-'))
	{
		if (*nptr == '-')
			minus = -minus;
		nptr++;
	}
	while ((*nptr >= '0') && (*nptr <= '9'))
		result = (result * 10) + (*nptr++ - '0');
	return (result * minus);
}

static int	ft_isspace(char c)
{
	if ((c == ' ') || (c == '\f') || (c == '\n')
		|| (c == '\r') || (c == '\t') || (c == '\v'))
		return (1);
	return (0);
}
