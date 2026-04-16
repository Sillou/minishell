/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   06_exit_builtin.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alubrano <alubrano@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 21:59:13 by alubrano          #+#    #+#             */
/*   Updated: 2026/04/05 00:31:25 by alubrano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
	Here to exit with and code
*/

static int	is_numeric_arg(char *s);

int	builtin_exit(t_ms *ms)
{
	int		argc;
	long	code;

	if (!ms->cmds || !*ms->cmds || !(*ms->cmds)->cmd)
		return (1);
	argc = 0;
	while ((*ms->cmds)->cmd[argc])
		argc++;
	ft_putendl_fd("exit", 1);
	if (argc >= 2 && !is_numeric_arg((*ms->cmds)->cmd[1]))
	{
		printf("minishell: exit: %s: numeric argument required\n",
			(*ms->cmds)->cmd[1]);
		ms->exit_code = 2;
		return (-1);
	}
	if (argc > 2)
		return (ft_putendl_fd("minishell: exit: too many arguments", 2), 1);
	if (argc == 2)
	{
		code = ft_atoi((*ms->cmds)->cmd[1]);
		ms->exit_code = ((code % 256) + 256) % 256;
	}
	return (-1);
}

void	free_redire(t_redir *r)
{
	t_redir	*tmp;

	while (r)
	{
		tmp = r->next;
		free(r->file);
		free(r);
		r = tmp;
	}
}

void	ft_free(t_ms *ms)
{
	int			k;
	t_envp		*cur;
	t_envp		*next;

	cur = *ms->envp;
	while (cur)
	{
		next = cur->next;
		free(cur->var);
		free(cur->content);
		free(cur);
		cur = next;
	}
	*ms->envp = NULL;
	free(ms->envp);
	ms->envp = NULL;
	if (ms->path != NULL)
	{
		k = 0;
		while (ms->path[k])
			free(ms->path[k++]);
		free(ms->path);
	}
}

static int	is_numeric_arg(char *s)
{
	int	i;

	if (!s || !s[0])
		return (0);
	i = 0;
	if (s[i] == '+' || s[i] == '-')
		i++;
	if (!s[i])
		return (0);
	while (s[i])
	{
		if (!ft_isdigit(s[i]))
			return (0);
		i++;
	}
	return (1);
}
