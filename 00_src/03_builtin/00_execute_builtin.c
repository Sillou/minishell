/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_builtin.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alubrano <alubrano@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/19 15:37:20 by dcresce           #+#    #+#             */
/*   Updated: 2026/03/22 17:52:16 by alubrano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/********************************************************
*					BUILTIN CHECK						*
********************************************************/

/*
	Execute all builtin whith
*/

int	builtin_exec(t_ms *ms)
{
	if (!ms || !ms->cmds || !*ms->cmds || !(*ms->cmds)->cmd)
		return (1);
	if (ft_strncmp((*ms->cmds)->cmd[0], "echo", 5) == 0)
		return (builtin_echo(ms));
	if (ft_strncmp((*ms->cmds)->cmd[0], "cd", 3) == 0)
		return (builtin_cd(ms));
	if (ft_strncmp((*ms->cmds)->cmd[0], "env", 4) == 0)
		return (builtin_env(ms));
	if (ft_strncmp((*ms->cmds)->cmd[0], "pwd", 4) == 0)
		return (builtin_pwd(ms));
	if (ft_strncmp((*ms->cmds)->cmd[0], "exit", 5) == 0)
		return (builtin_exit(ms));
	if (ft_strncmp((*ms->cmds)->cmd[0], "export", 7) == 0)
		return (builtin_export(ms));
	if (ft_strncmp((*ms->cmds)->cmd[0], "unset", 6) == 0)
		return (builtin_unset(ms, (*ms->cmds)->cmd));
	ft_putendl_fd("Error builtin", 2);
	return (1);
}
