/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   00_pars.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alubrano <alubrano@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/31 15:17:18 by dcresce           #+#    #+#             */
/*   Updated: 2026/04/05 01:38:03 by alubrano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*check_cmd(char *str)
{
	char	*pairs;

	if (!str)
		return (NULL);
	pairs = check_pairs(str);
	if (!pairs)
		return (NULL);
	if (!syntax_check(pairs))
	{
		if (pairs != str)
			free(pairs);
		return (NULL);
	}
	return (pairs);
}

t_ms	**pars(char *cmd, char **envp)
{
	t_ms	**ms;

	if (!cmd)
	{
		ms = ft_calloc(1, sizeof(*ms));
		*ms = ft_calloc(1, sizeof(**ms));
		(*ms)->envp = make_envp(envp);
		return (ms);
	}
	ms = ft_calloc(1, sizeof(*ms));
	*ms = ft_calloc(1, sizeof(**ms));
	(*ms)->envp = make_envp(envp);
	(*ms)->cmds = ast(cmd, ms);
	if (!(*ms)->cmds)
		return (free_ms(ms), NULL);
	return (ms);
}
