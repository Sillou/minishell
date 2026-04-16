/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   04_format_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alubrano <alubrano@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/31 18:23:49 by dcresce           #+#    #+#             */
/*   Updated: 2026/04/05 01:06:43 by alubrano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*single_quote(char *cmd, int *i, int *j);
static char	*double_quotes(char *cmd, int *i, int *j, t_ms *ms);
static char	*erase_char(char *str, int i, int j);

char	*handle_quotes(char *cmd, t_ms *ms)
{
	int		i;
	int		j;
	char	*temp;

	i = 0;
	while (cmd[i])
	{
		while ((cmd[i] != '\'') && (cmd[i] != '"') && cmd[i])
			i++;
		if (cmd[i] == '\'')
		{
			temp = single_quote(cmd, &i, &j);
			free(cmd);
			cmd = ft_strdup(temp);
			free(temp);
		}
		if (cmd[i] == '"')
		{
			temp = double_quotes(cmd, &i, &j, ms);
			free(cmd);
			cmd = ft_strdup(temp);
			free(temp);
		}
	}
	return (cmd);
}

static char	*single_quote(char *cmd, int *i, int *j)
{
	char	*s;

	s = NULL;
	*j = *i + 1;
	while (cmd[*j] != '\'' && cmd[*j])
		(*j)++;
	if (s)
		free(s);
	s = erase_char(cmd, *i, *j);
	*i = --(*j);
	return (s);
}

static char	*double_quotes(char *cmd, int *i, int *j, t_ms *ms)
{
	char	*s;
	char	*temp;

	s = NULL;
	*j = *i + 1;
	temp = ft_strdup(cmd);
	while (temp[*j] != '"' && temp[*j])
	{
		if (temp[*j] == '$')
		{
			free(temp);
			temp = handle_expand(cmd, ms);
			while (temp[*j] != '"' && temp[*j == ' '] && temp[*j])
				(*j)++;
		}
		else
			(*j)++;
	}
	s = erase_char(temp, *i, *j);
	free(temp);
	*i = --(*j);
	return (s);
}

static char	*erase_char(char *str, int c1, int c2)
{
	int		len;
	int		i;
	int		j;
	char	*s;

	if (!str)
		return (NULL);
	len = ft_strlen(str);
	if (c1 > len || c2 > len)
		return (NULL);
	s = calloc(len - 1, sizeof(char));
	i = 0;
	j = 0;
	while (i < len && str[i])
	{
		if ((i != c1) && (i != c2))
			s[j++] = str[i++];
		else
			i++;
	}
	return (s);
}

char	*get_expanded(char *s, t_ms *ms, int *i, int *j)
{
	*i = 0;
	while (s[*i] && (s[*i] != '$'))
		(*i)++;
	*j = *i + 1;
	if (s[*j] == '?')
		return (ft_itoa(ms->exit_code));
	return (expand_helper(s, ms, i, j));
}
