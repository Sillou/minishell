/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   09_wilcard_expand.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alubrano <alubrano@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/02 23:30:00 by alubrano          #+#    #+#             */
/*   Updated: 2026/04/03 10:16:08 by alubrano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	count_expanded_size(char **cmd);
static void	transfer_expansion(char **result, char **exp, int *pos);
static char	**fill_expanded(char **result, char **cmd);

char	**expand_wildcards_in_cmd(char **cmd)
{
	char	**result;
	int		total;

	if (!cmd)
		return (NULL);
	total = count_expanded_size(cmd);
	result = ft_calloc(total + 1, sizeof(char *));
	if (!result)
		return (NULL);
	return (fill_expanded(result, cmd));
}

static int	count_expanded_size(char **cmd)
{
	char	**exp;
	int		total;
	int		i;
	int		j;

	total = 0;
	i = 0;
	while (cmd[i])
	{
		exp = expand_wildcard(cmd[i]);
		if (!exp)
			total++;
		else
		{
			j = 0;
			while (exp[j++])
				total++;
			free_wildcard_matches(exp);
		}
		i++;
	}
	return (total);
}

static void	transfer_expansion(char **result, char **exp, int *pos)
{
	int	j;

	j = 0;
	while (exp[j])
		result[(*pos)++] = exp[j++];
	free(exp);
}

static char	**fill_expanded(char **result, char **cmd)
{
	char	**exp;
	int		pos;
	int		i;

	pos = 0;
	i = 0;
	while (cmd[i])
	{
		exp = expand_wildcard(cmd[i]);
		if (!exp)
		{
			result[pos++] = ft_strdup(cmd[i]);
			if (!result[pos - 1])
				return (free_wildcard_matches(result), NULL);
		}
		else
			transfer_expansion(result, exp, &pos);
		i++;
	}
	return (result);
}
