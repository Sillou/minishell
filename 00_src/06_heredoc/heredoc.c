/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alubrano <alubrano@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/31 14:38:21 by dcresce           #+#    #+#             */
/*   Updated: 2026/04/08 11:24:29 by alubrano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	go_child(const char *delim, int fd0, int fd1, t_ms *ms);
static int	create_heredoc_fd(const char *delim, t_ms *ms);
static void	heredoc_child(const char *de, int fd, t_ms *ms, bool expand);
static void	ft_free_here(char *tofree, char *onfree);

int	ft_here_doc(char *del, t_ms *ms)
{
	int	fd;

	if (!del)
		return (-1);
	heredoc_lock();
	fd = create_heredoc_fd(del, ms);
	unlock_ctrlc();
	return (fd);
}

static void	heredoc_child(const char *de, int fd, t_ms *ms, bool expand)
{
	char	*line;
	char	*tmp;

	line = NULL;
	while (1)
	{
		line = readline("> ");
		if (!line)
			no_line(de);
		if (ft_strncmp(line, de, ft_strlen(de)) == 0
			&& (line[ft_strlen(de)] == '\n' || line[ft_strlen(de)] == '\0'))
		{
			free(line);
			break ;
		}
		if (expand && ft_strchr(line, '$'))
			tmp = handle_expand(line, ms);
		else
			tmp = ft_strdup(line);
		ft_putendl_fd(tmp, fd);
		ft_free_here(line, tmp);
	}
	close(fd);
	exit(0);
}

static int	create_heredoc_fd(const char *delim, t_ms *ms)
{
	int		fds[2];
	pid_t	pid;
	int		status;
	int		rd;

	rd = -1;
	if (pipe(fds) == -1)
		return (-1);
	pid = fork();
	if (pid == -1)
	{
		close(fds[0]);
		close(fds[1]);
		return (-1);
	}
	if (pid == 0)
		go_child(delim, fds[0], fds[1], ms);
	close(fds[1]);
	heredoc_lock();
	status = wait_one_child(pid);
	ms->exit_code = status;
	if (status == 130)
		return (close(fds[0]), -1);
	rd = fds[0];
	return (rd);
}

static void	go_child(const char *delim, int fd0, int fd1, t_ms *ms)
{
	bool	expand;

	expand = !(delim[0] == '\'' || delim[0] == '"');
	unlock_ctrlc();
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_IGN);
	close(fd0);
	heredoc_child(delim, fd1, ms, expand);
}

static void	ft_free_here(char *tofree, char *onfree)
{
	if (!tofree && !onfree)
		return ;
	if (tofree)
		free(tofree);
	if (onfree)
		free(onfree);
}
