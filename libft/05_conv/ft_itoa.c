/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcresce <dcresce@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 14:50:46 by dcresce           #+#    #+#             */
/*   Updated: 2025/11/18 14:50:46 by dcresce          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"

static size_t	get_lenght(int n);

char	*ft_itoa(int n)
{
	char	*result;
	size_t	size;
	long	_n;

	_n = (long)n;
	size = get_lenght(_n);
	result = malloc(sizeof(char) * (size + 1));
	if (!result)
		return (NULL);
	if (_n < 0)
	{
		_n = -_n;
		result[0] = '-';
	}
	if (_n == 0)
		result[0] = '0';
	result[size] = '\0';
	while ((_n % 10) || (_n / 10))
	{
		result[--size] = (_n % 10) + '0';
		_n /= 10;
	}
	return (result);
}

static size_t	get_lenght(int n)
{
	size_t	lenght;

	lenght = 0;
	if (n <= 0)
		lenght++;
	while (n % 10 || n / 10)
	{
		lenght++;
		n /= 10;
	}
	return (lenght);
}
