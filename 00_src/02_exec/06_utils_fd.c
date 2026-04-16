/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   06_utils_fd.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alubrano <alubrano@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/12 13:49:23 by alubrano          #+#    #+#             */
/*   Updated: 2026/03/27 12:01:29 by alubrano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* splitting PATH */
void	ft_get_path(t_ms *ms)
{
	t_envp	*node;
	char	*line;

	line = NULL;
	node = check_exist(*ms->envp, "PATH");
	if (node && node->content)
		line = node->content;
	if (!line)
	{
		ms->path = NULL;
		return ;
	}
	ms->path = ft_split(line, ':');
	if (!ms->path)
		msg_error("Path", "split error", 1, ms);
}

/* Close fd if it is a valid descriptor. */
void	close_fd(int fd)
{
	if (fd >= 0)
		close(fd);
}

/* Execute command PATH and execve. */
void	ft_execut(t_ms *ms, t_ast *cmd)
{
	char	*cmd_path;
	char	*temp;
	int		i;
	char	**t_cmd;
	char	**env_array;

	t_cmd = cmd->cmd;
	if (!t_cmd)
		msg_error("t_cmd", "split error", 2, ms);
	env_array = envp_to_array(*ms->envp);
	i = -1;
	if (t_cmd[0] && access(t_cmd[0], X_OK) == 0)
		execve(t_cmd[0], t_cmd, env_array);
	while (ms->path && ms->path[++i])
	{
		temp = ft_strjoin(ms->path[i], "/");
		cmd_path = ft_strjoin(temp, t_cmd[0]);
		free(temp);
		if (access(cmd_path, X_OK) == 0)
			execve(cmd_path, t_cmd, env_array);
		free(cmd_path);
	}
	msg_error(t_cmd[0], "command not found", 1, ms);
	free_env_array(env_array);
	exit(127);
}

int	wait_one_child(pid_t pid)
{
	int	status;

	if (waitpid(pid, &status, 0) < 0)
		return (perror("waitpid"), 1);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
	{
		if (WTERMSIG(status) == SIGQUIT)
			write(1, "Quit (core dumped)\n", 19);
		return (128 + WTERMSIG(status));
	}
	return (1);
}

int	must_run_builtin_in_parent(t_exe_cmd *execmd)
{
	if (!execmd || !execmd->node || !execmd->node->built_in)
		return (0);
	if (execmd->in_pipe)
		return (0);
	return (is_parent_builtin(execmd->node));
}
