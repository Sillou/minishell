/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   04_env_builtin.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alubrano <alubrano@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 20:28:29 by alubrano          #+#    #+#             */
/*   Updated: 2026/04/05 00:43:36 by alubrano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_envp	*find_envp(t_envp *envp, char *var);

/*
	print envp
*/

int	builtin_env(t_ms *ms)
{
	t_envp	*current;

	if (!ms || !*ms->envp)
		return (0);
	current = *ms->envp;
	while (current)
	{
		if (current->content)
		{
			if (!ft_strcmp(current->var, "PATH") && count_env(*ms->envp) < 10)
				ft_putstr_fd("_", 1);
			else
				ft_putstr_fd(current->var, 1);
			ft_putchar_fd('=', 1);
			ft_putendl_fd(current->content, 1);
		}
		current = current->next;
	}
	return (0);
}

static t_envp	*find_envp(t_envp *envp, char *var)
{
	while (envp)
	{
		if (envp->var && ft_strncmp(envp->var, var, ft_strlen(var) + 1) == 0)
			return (envp);
		envp = envp->next;
	}
	return (NULL);
}

void	add_shlvl(t_ms *ms)
{
	int		j;
	char	*new_value;
	t_envp	*node;

	if (!ms || !*ms->envp)
		return ;
	node = find_envp(*ms->envp, "SHLVL");
	if (!node || !node->content)
		j = 0;
	else
		j = ft_atoi(node->content);
	j += 1;
	new_value = ft_itoa(j);
	if (!new_value)
		return ;
	new_set_envp(ms, "SHLVL", new_value);
	free(new_value);
}

int	count_env(t_envp *envp)
{
	int	i;

	i = 0;
	while (envp)
	{
		envp = envp->next;
		i++;
	}
	return (i);
}

void	if_no_env(t_ms *ms)
{
	char	*current;
	char	buffer[PATH_MAX];

	current = getcwd(buffer, PATH_MAX);
	new_set_envp(ms, "PWD", current);
	new_set_envp(ms, "OLDPWD", NULL);
	new_set_envp(ms, "PATH", PATH);
}
