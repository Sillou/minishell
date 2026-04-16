/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   07_export_builtin.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alubrano <alubrano@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 22:40:07 by alubrano          #+#    #+#             */
/*   Updated: 2026/03/28 17:45:54 by alubrano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int		split_var_val(char *arg, char **var, char **val);
static void		add_new_var(t_ms *ms, t_envp *new_node);
static t_envp	*create_env_node(char *var, char *value);

int	builtin_export(t_ms *ms)
{
	char	*variable;
	char	*value;
	int		i;

	if (!ms->envp)
		return (1);
	if (!ms->cmds || !*ms->cmds || !(*ms->cmds)->cmd)
		return (1);
	if (!(*ms->cmds)->cmd[1])
		return (export_sort(ms));
	i = 0;
	while ((*ms->cmds)->cmd[++i])
	{
		variable = NULL;
		value = NULL;
		if (split_var_val((*ms->cmds)->cmd[i], &variable, &value) != 0)
			return (free(value), free(variable), 1);
		new_set_envp(ms, variable, value);
		free(variable);
		free(value);
	}
	return (0);
}

static int	split_var_val(char *arg, char **var, char **val)
{
	char	*egal;

	if (!arg)
		return (1);
	*var = ft_strdup(arg);
	if (!*var)
		return (1);
	if (!is_valid(arg))
	{
		egal = ft_strjoin("minishell : export: ", arg);
		if (!egal)
			return (1);
		msg_error(egal, "not a valid identifier", 0, NULL);
		free(egal);
		return (1);
	}
	egal = ft_strchr(*var, '=');
	if (!egal)
		return (0);
	*egal = '\0';
	*val = ft_strdup(egal + 1);
	if (!*val)
		return (1);
	return (0);
}

void	new_set_envp(t_ms *ms, char	*var, char	*value)
{
	t_envp	*node;
	char	*new_content;

	if (!ms || !var)
		return ;
	node = check_exist(*ms->envp, var);
	if (value)
		new_content = ft_strdup(value);
	else
		new_content = ft_strdup("");
	if (!new_content)
		return ;
	if (node)
	{
		free(node->content);
		node->content = new_content;
		return ;
	}
	add_new_var(ms, create_env_node(var, new_content));
	free(new_content);
}

static void	add_new_var(t_ms *ms, t_envp *new_node)
{
	t_envp	*last;

	if (!ms || !new_node)
		return ;
	if (!*ms->envp)
	{
		*ms->envp = new_node;
		return ;
	}
	last = *ms->envp;
	while (last->next)
		last = last->next;
	last->next = new_node;
}

static t_envp	*create_env_node(char *var, char *value)
{
	t_envp	*node;

	if (!var)
		return (NULL);
	node = malloc(sizeof(t_envp));
	if (!node)
		return (NULL);
	node->var = ft_strdup(var);
	if (!node->var)
	{
		free(node);
		return (NULL);
	}
	if (value)
		node->content = ft_strdup(value);
	else
		node->content = ft_strdup("");
	if (!node->content)
	{
		free(node->var);
		free(node);
		return (NULL);
	}
	node->next = NULL;
	return (node);
}
