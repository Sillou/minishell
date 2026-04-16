/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   00_ast_exec.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alubrano <alubrano@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 16:28:14 by alubrano          #+#    #+#             */
/*   Updated: 2026/04/03 11:19:15 by alubrano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	exec_cmd_child(t_exe_cmd *execmd);
static int	exec_external(t_exe_cmd *execmd);
static void	expand_cmd_args(t_exe_cmd *execmd);

int	exec_ast(t_ms *ms, t_ast *node)
{
	t_exe_cmd	exe_cmd;

	if (!ms || !node)
		return (1);
	if (node->op == OP_AND)
		return (exec_and_node(ms, node));
	if (node->op == OP_OR)
		return (exec_or_node(ms, node));
	if (node->op == OP_PIPE)
		return (exec_pipe_node(ms, node));
	if (node->op == OP_SUB)
		return (exec_sub_node(ms, node));
	exe_cmd.ms = ms;
	exe_cmd.node = node;
	exe_cmd.in_fd = -1;
	exe_cmd.out_fd = -1;
	exe_cmd.in_pipe = 0;
	return (exec_cmd_node(&exe_cmd));
}

int	exec_cmd_node(t_exe_cmd *execmd)
{
	pid_t	pid;
	int		status;

	if (!execmd || !execmd->ms || !execmd->node)
		return (1);
	if (!execmd->node->cmd || !execmd->node->cmd[0])
		return (0);
	expand_cmd_args(execmd);
	if (must_run_builtin_in_parent(execmd))
		return (exec_parent_builtin(execmd));
	write_lock();
	pid = fork();
	if (pid < 0)
		return (perror("fork"), 1);
	if (pid == 0)
		exec_cmd_child(execmd);
	status = wait_one_child(pid);
	unlock_ctrlc();
	execmd->ms->exit_code = status;
	return (status);
}

static void	exec_cmd_child(t_exe_cmd *execmd)
{
	int		status;
	char	**expanded;

	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	setup_fd(execmd->node, execmd->in_fd, execmd->out_fd);
	close_fd(execmd->in_fd);
	close_fd(execmd->out_fd);
	expanded = expand_wildcards_in_cmd(execmd->node->cmd);
	if (expanded)
	{
		free_split(execmd->node->cmd);
		execmd->node->cmd = expanded;
	}
	if (execmd->node->built_in)
		status = run_builtin_on_node(execmd);
	else
		status = exec_external(execmd);
	exit(status);
}

static int	exec_external(t_exe_cmd *execmd)
{
	if (!execmd || !execmd->ms || !execmd->node)
		return (1);
	ft_execut(execmd->ms, execmd->node);
	return (127);
}

static void	expand_cmd_args(t_exe_cmd *execmd)
{
	char	**cmd;
	char	*expanded;
	int		i;

	cmd = execmd->node->cmd;
	if (!cmd || !cmd[0])
		return ;
	expanded = format_cmd(cmd[0], execmd->ms, true);
	free(cmd[0]);
	cmd[0] = expanded;
	execmd->node->built_in = is_builtin(cmd[0]);
	i = 1;
	while (cmd[i])
	{
		expanded = format_cmd(cmd[i], execmd->ms, false);
		free(cmd[i]);
		cmd[i] = expanded;
		i++;
	}
}
