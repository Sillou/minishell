/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   05_exec_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alubrano <alubrano@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 18:37:35 by alubrano          #+#    #+#             */
/*   Updated: 2026/04/04 23:59:49 by alubrano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	run_builtin_on_node(t_exe_cmd *execmd)
{
	t_ast	*save;
	int		status;

	if (!execmd || !execmd->ms || !execmd->node)
		return (1);
	save = *execmd->ms->cmds;
	*execmd->ms->cmds = execmd->node;
	status = builtin_exec(execmd->ms);
	if (status != -1)
		execmd->ms->exit_code = status;
	*execmd->ms->cmds = save;
	return (status);
}

int	exec_parent_builtin(t_exe_cmd *execmd)
{
	int	stdin_save;
	int	stdout_save;
	int	status;

	stdin_save = dup(STDIN_FILENO);
	stdout_save = dup(STDOUT_FILENO);
	if (stdin_save < 0 || stdout_save < 0)
		return (perror("dup"), 1);
	setup_fd(execmd->node, execmd->in_fd, execmd->out_fd);
	close_fd(execmd->in_fd);
	close_fd(execmd->out_fd);
	status = run_builtin_on_node(execmd);
	dup2(stdin_save, STDIN_FILENO);
	dup2(stdout_save, STDOUT_FILENO);
	close_fd(stdin_save);
	close_fd(stdout_save);
	if (status != -1)
		execmd->ms->exit_code = status;
	return (status);
}

int	is_parent_builtin(t_ast *node)
{
	char	*cmd;

	if (!node || !node->cmd || !node->cmd[0])
		return (0);
	cmd = node->cmd[0];
	if (ft_strncmp(cmd, "cd", 3) == 0)
		return (1);
	if (ft_strncmp(cmd, "export", 7) == 0)
		return (1);
	if (ft_strncmp(cmd, "unset", 6) == 0)
		return (1);
	if (ft_strncmp(cmd, "exit", 5) == 0)
		return (1);
	return (0);
}
