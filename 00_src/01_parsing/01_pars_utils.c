/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   01_pars_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcresce <dcresce@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 19:01:10 by dcresce           #+#    #+#             */
/*   Updated: 2026/04/08 19:01:10 by dcresce          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	char_check(char *s, int i);

int	syntax_check(char *s)
{
	int	i;

	i = 0;
	if (ft_strchr("|&", s[0]))
		return (ft_printf("%s `%c'\n", SYN_ERR, s[0]), 0);
	while (s[i])
	{
		if (s[i] == '|')
			if ((s[i + 1] != '|' && ft_strchr("&<>", s[i + 1]))
				|| (s[i + 1] == '|' && (s[i + 2] == '|'
						|| ft_strchr("&<>", s[i + 2]))))
				return (ft_printf("%s `|'\n", SYN_ERR), 0);
		if ((s[i] == '&') && (s[i - 1] != '&'))
			if (s[i + 1] != '&' || s[i + 2] == '&')
				return (ft_printf("%s `&'\n", SYN_ERR), 0);
		if (s[i] == '(' && ft_strchr("|&<>)", s[i + 1]))
			return (ft_printf("%s `('\n", SYN_ERR), 0);
		if (s[i] == ')' && ft_strchr("|&<>", s[i - 1]))
			return (ft_printf("%s `)'\n", SYN_ERR), 0);
		if (!char_check(s, i))
			return (0);
		i++;
	}
	return (1);
}

static int	char_check(char *s, int i)
{
	char	tmp[2];

	if ((s[i] == '<' || s[i] == '>'))
	{
		tmp[0] = s[i];
		tmp[1] = '\0';
		if (s[i + 1] == s[i] && s[i + 2] == s[i])
			return (msg_error(SYN_ERR, tmp, 0, NULL), 0);
		if (s[i + 1] != s[i] && ft_strchr("()&|<>", s[i + 1]))
			return (msg_error(SYN_ERR, tmp, 0, NULL), 0);
		if (s[i + 1] == s[i] && ft_strchr("()&|<>", s[i + 2]))
			return (msg_error(SYN_ERR, tmp, 0, NULL), 0);
	}
	return (1);
}

void	free_ms(t_ms **ms)
{
	if ((*ms)->envp)
		clear_envp((*ms)->envp);
	if ((*ms)->cmds)
		free_ast((*ms)->cmds);
	if ((*ms)->path)
		free_split((*ms)->path);
	if ((*ms)->c_dir)
		free((*ms)->c_dir);
	free(*ms);
	free(ms);
}
