/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alubrano <alubrano@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/13 22:24:40 by alubrano          #+#    #+#             */
/*   Updated: 2026/03/18 01:01:43 by alubrano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	handle_dollar(t_ms *ms, char *s, int *k);
static char	*get_env_val(t_ms *ms, const char *name);

void	print_single_quoted(const char *s)
{
	size_t	l;
	char	*sub;

	l = ft_strlen(s);
	if (l < 2)
		return ;
	sub = ft_substr(s, 1, l - 2);
	if (sub)
	{
		ft_putstr_fd(sub, 1);
		free(sub);
	}
}

char	*strip_double_quotes(const char *s)
{
	size_t	l;

	if (!s || s[0] != '"')
		return (NULL);
	l = ft_strlen(s);
	if (l >= 2 && s[l - 1] == '"')
		return (ft_substr(s, 1, l - 2));
	return (NULL);
}

void	check_dollar(t_ms *ms, char *s, int *j)
{
	int	i;

	i = *j;
	if (s[i + 1] == '?')
	{
		ft_putnbr_fd(ms->exit_code, 1);
		*j = i + 2;
		return ;
	}
	else
		handle_dollar(ms, s, j);
}

static void	handle_dollar(t_ms *ms, char *s, int *k)
{
	char	*val;
	char	*name;
	int		j;
	int		i;

	i = *k;
	j = i + 1;
	while (s[j] && (ft_isalnum(s[j]) || s[j] == '_'))
		j++;
	if (j == i + 1)
	{
		ft_putchar_fd('$', 1);
		*k = i + 1;
		return ;
	}
	name = ft_substr(s, i + 1, j - (i + 1));
	val = get_env_val(ms, name);
	if (val)
		ft_putstr_fd(val, 1);
	free(name);
	*k = j;
}

static char	*get_env_val(t_ms *ms, const char *name)
{
	t_envp	*current;

	if (!ms || !*ms->envp || !name)
		return (NULL);
	current = *ms->envp;
	while (current)
	{
		if (current->var && ft_strncmp(current->var, name,
				ft_strlen(name) + 1) == 0)
			return (current->content);
		current = current->next;
	}
	return (NULL);
}
