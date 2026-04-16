/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcresce <dcresce@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 14:59:35 by dcresce           #+#    #+#             */
/*   Updated: 2025/12/19 15:00:11 by dcresce          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"

static int		countwords(char const *s, char c);
static size_t	wordsize(char const *s, char c);
static char		**free_split_until(char **list, int upto);

char	**ft_split(char const *s, char c)
{
	char	**list;
	int		words_nbr;
	int		i;
	int		j;

	words_nbr = countwords(s, c);
	list = malloc(sizeof(char *) * (words_nbr + 1));
	if (!list)
		return (NULL);
	i = 0;
	while (i < words_nbr)
	{
		list[i] = malloc(sizeof(char) * (wordsize(s, c) + 1));
		if (!list[i])
			return (free_split_until(list, i));
		j = 0;
		while (*s == c)
			s++;
		while (*s && (*s != c))
			list[i][j++] = *s++;
		list[i][j] = '\0';
		i++;
	}
	list[i] = 0;
	return (list);
}

static int	countwords(char const *s, char c)
{
	int	i;
	int	count;

	i = 0;
	count = 0;
	while (s[i] != '\0')
	{
		if (s[i] == c)
			i++;
		else
		{
			count++;
			while (s[i] && s[i] != c)
				i++;
		}
	}
	return (count);
}

static size_t	wordsize(char const *s, char c)
{
	size_t	len;

	len = 0;
	while (*s == c)
		s++;
	while (s[len] && (s[len] != c))
		len++;
	return (len);
}

static char	**free_split_until(char **list, int upto)
{
	while (--upto >= 0)
		free(list[upto]);
	free(list);
	return (NULL);
}
