/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   02_exe_sub.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alubrano <alubrano@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 11:46:48 by alubrano          #+#    #+#             */
/*   Updated: 2026/04/05 00:17:09 by alubrano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	exec_sub_node(t_ms *ms, t_ast *node)
{
	pid_t	pid;
	int		status;

	if (!ms || !node || !node->left)
		return (1);
	write_lock();
	pid = fork();
	if (pid < 0)
		return (perror("fork"), 1);
	if (pid == 0)
	{
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		status = exec_ast(ms, node->left);
		if (status == -1)
			status = ms->exit_code;
		exit(status);
	}
	status = wait_one_child(pid);
	unlock_ctrlc();
	ms->exit_code = status;
	return (status);
}
