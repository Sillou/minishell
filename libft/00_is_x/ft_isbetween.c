/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isbetween.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcresce <dcresce@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 20:53:47 by dcresce           #+#    #+#             */
/*   Updated: 2026/04/01 20:54:47 by dcresce          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"

int	ft_isbetween(int n, int min, int max)
{
	if (min >= max)
		return (0);
	if ((n >= min) && (n <= max))
		return (1);
	return (0);
}
