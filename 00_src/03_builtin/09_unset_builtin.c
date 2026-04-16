/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset_builtin.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alubrano <alubrano@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 21:45:24 by alubrano          #+#    #+#             */
/*   Updated: 2026/03/19 09:10:28 by alubrano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	unset_value(t_ms *ms, t_envp *target);

/*
	here to remove environment variable(s)
	add variable
	modified variable
	rm variable
*/

int	builtin_unset(t_ms *ms, char **args)
{
	t_envp	*found;
	char	*egal;
	char	*name;
	int		i;

	i = 0;
	if (!ms || !args || !ms->envp)
		return (0);
	while (args[++i])
	{
		name = ft_strdup(args[i]);
		if (!name)
			return (1);
		egal = ft_strchr(name, '=');
		if (egal)
			*egal = '\0';
		if (!name)
			return (1);
		found = check_exist(*ms->envp, name);
		free(name);
		if (!found)
			return (0);
		unset_value(ms, found);
	}
	return (0);
}

static void	unset_value(t_ms *ms, t_envp *target)
{
	t_envp	*prev;
	t_envp	*cur;

	if (!ms || !target)
		return ;
	prev = NULL;
	cur = *ms->envp;
	while (cur && cur != target)
	{
		prev = cur;
		cur = cur->next;
	}
	if (!cur)
		return ;
	if (prev)
		prev->next = cur->next;
	else
		*ms->envp = cur->next;
	free(cur->var);
	free(cur->content);
	free(cur);
}

t_envp	*check_exist(t_envp *envp, char *var)
{
	if (!var)
		return (NULL);
	while (envp)
	{
		if (envp->var && ft_strncmp(envp->var, var, ft_strlen(var) + 1) == 0)
			return (envp);
		envp = envp->next;
	}
	return (NULL);
}
