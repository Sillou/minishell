/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_builtin.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alubrano <alubrano@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/07 19:26:22 by alubrano          #+#    #+#             */
/*   Updated: 2026/04/05 19:24:34 by alubrano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	print_error(char *str, char *error_msg)
{
	char	*msg;
	char	*tmp;

	if (str != NULL)
	{
		msg = ft_strjoin(str, ": ");
		if (!msg)
			return (1);
		tmp = ft_strjoin(msg, error_msg);
		free(msg);
		if (!tmp)
			return (1);
		msg = tmp;
	}
	else
		msg = ft_strdup(error_msg);
	if (msg)
	{
		ft_putendl_fd(msg, 2);
		free(msg);
	}
	return (1);
}

int	msg_error(char *str, char *error_msg, int nb_error, t_ms *ms)
{
	(void)ms;
	print_error(str, error_msg);
	if (ms && nb_error == 1)
		ft_exit(ms);
	if (nb_error == 2)
		exit(1);
	return (1);
}

int	is_valid(char *arg)
{
	int	i;

	if (!arg || !arg[0])
		return (0);
	if (!(ft_isalpha(arg[0]) || arg[0] == '_'))
		return (0);
	i = 1;
	while (arg[i] && arg[i] != '=')
	{
		if (!(ft_isalnum(arg[i]) || arg[i] == '_' || arg[i] == '='))
			return (0);
		i++;
	}
	return (1);
}

/*
	Deux fonctions qui permettent de free les allocs
*/

void	ft_exit(t_ms *ms)
{
	if (!ms)
		return ;
	if (ms->cmds)
	{
		if (*ms->cmds)
		{
			free_cmd(*ms->cmds);
			*ms->cmds = NULL;
		}
		free(ms->cmds);
		ms->cmds = NULL;
	}
	ft_free(ms);
	free(ms);
}

void	free_cmd(t_ast *n)
{
	int	k;

	if (!n)
		return ;
	free_cmd(n->left);
	free_cmd(n->right);
	if (n->cmd)
	{
		k = 0;
		while (n->cmd[k])
			free(n->cmd[k++]);
		free(n->cmd);
	}
	if (n->redir)
		free_redire(n->redir);
	free(n);
}
