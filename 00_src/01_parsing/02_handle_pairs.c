/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   02_handle_pairs.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alubrano <alubrano@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 20:41:15 by dcresce           #+#    #+#             */
/*   Updated: 2026/04/07 21:03:20 by alubrano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	missing_pair(const char *s);
static char	*join_eof(char *str, char del);

char	*check_pairs(char *str)
{
	int		need;

	need = missing_pair(str);
	while (need != 0)
	{
		need = missing_pair(str);
		if (need == -1)
			return (msg_error(SYN_ERR, ")", 0, NULL), NULL);
		if (need == 1)
			str = join_eof(str, ')');
		else if (need == '"')
			str = join_eof(str, '"');
		else if (need == '\'')
			str = join_eof(str, '\'');
		if (!str)
			return (NULL);
	}
	if ((str[ft_strlen(str) - 1] == '&') || str[ft_strlen(str) - 1] == '|')
		str = join_eof(str, '\n');
	if (!str)
		return (NULL);
	return (str);
}

static int	missing_pair(const char *str)
{
	int		cnt[2];
	char	quote;

	cnt[0] = 0;
	cnt[1] = -1;
	quote = '\0';
	while (str[++cnt[1]])
		if (str[cnt[1]] == quote)
			quote = '\0';
	else
	{
		if (!quote && (str[cnt[1]] == '('))
			cnt[0]++;
		if (!quote && (str[cnt[1]] == ')'))
			cnt[0]--;
		if (!quote && ((str[cnt[1]] == '"') || (str[cnt[1]] == '\'')))
			quote = str[cnt[1]];
		if (cnt[0] < 0)
			return (-1);
	}
	if (quote)
		return ((int)quote);
	if (cnt[0] > 0)
		return (1);
	return (0);
}

static char	*join_eof(char *str, char del)
{
	char	*new;
	char	*tmp;
	char	*res;

	if ((del == '"') || (del == '\''))
		tmp = ft_strjoin(str, "\n");
	else
		tmp = ft_strdup(str);
	while (1)
	{
		new = NULL;
		new = readline("> ");
		if (!new)
			return (free(tmp), NULL);
		res = ft_strjoin(tmp, new);
		free(tmp);
		if (del == '\n' || ft_strchr(new, del))
		{
			free(new);
			break ;
		}
		free(new);
		tmp = res;
	}
	return (res);
}
