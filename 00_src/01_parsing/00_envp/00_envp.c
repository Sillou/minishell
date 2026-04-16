/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   00_envp.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcresce <dcresce@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 19:15:45 by dcresce           #+#    #+#             */
/*   Updated: 2026/03/10 19:15:45 by dcresce          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_envp	**make_envp(char **envp)
{
	t_envp	**env;
	int		i;
	int		check;

	if (!envp)
		return (NULL);
	env = ft_calloc(1, sizeof(t_envp *));
	i = 0;
	while (envp[i])
	{
		check = add_var(env, new_var(envp[i]));
		if (!check)
			return (NULL);
		i++;
	}
	return (env);
}
