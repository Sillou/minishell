/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   01_exe_pipe.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alubrano <alubrano@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 11:46:48 by alubrano          #+#    #+#             */
/*   Updated: 2026/04/05 00:16:50 by alubrano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	init_pipe_node(t_pipe *fd);
static void	close_pipe(t_pipe *fd);
static void	pipe_left_child(t_ms *ms, t_ast *node, t_pipe *fd);
static void	pipe_right_child(t_ms *ms, t_ast *node, t_pipe *fd);

int	exec_pipe_node(t_ms *ms, t_ast *node)
{
	t_pipe	fd;

	if (!node || !node->left || !node->right)
		return (1);
	if (init_pipe_node(&fd) != 0)
		return (1);
	write_lock();
	fd.left_pid = fork();
	if (fd.left_pid < 0)
		return (close_pipe(&fd), perror("fork"), 1);
	if (fd.left_pid == 0)
		pipe_left_child(ms, node, &fd);
	fd.righ_pid = fork();
	if (fd.righ_pid < 0)
		return (close_pipe(&fd), perror("fork"), 1);
	if (fd.righ_pid == 0)
		pipe_right_child(ms, node, &fd);
	close_pipe(&fd);
	wait_one_child(fd.left_pid);
	unlock_ctrlc();
	return (wait_one_child(fd.righ_pid));
}

static int	init_pipe_node(t_pipe *fd)
{
	if (!fd)
		return (1);
	fd->left_pid = -1;
	fd->righ_pid = -1;
	if (pipe(fd->fds) < 0)
		return (perror("pipe"), 1);
	return (0);
}

static void	close_pipe(t_pipe *fd)
{
	if (!fd)
		return ;
	close_fd(fd->fds[0]);
	close_fd(fd->fds[1]);
}

static void	pipe_left_child(t_ms *ms, t_ast *node, t_pipe *fd)
{
	int	status;

	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	close_fd(fd->fds[0]);
	dup2(fd->fds[1], STDOUT_FILENO);
	close_fd(fd->fds[1]);
	status = exec_ast(ms, node->left);
	if (status == -1)
		status = ms->exit_code;
	exit(status);
}

static void	pipe_right_child(t_ms *ms, t_ast *node, t_pipe *fd)
{
	int	status;

	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	close_fd(fd->fds[1]);
	dup2(fd->fds[0], STDIN_FILENO);
	close_fd(fd->fds[0]);
	status = exec_ast(ms, node->right);
	if (status == -1)
		status = ms->exit_code;
	exit(status);
}
