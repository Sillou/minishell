/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   04_redir.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alubrano <alubrano@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/22 11:54:49 by alubrano          #+#    #+#             */
/*   Updated: 2026/03/26 20:29:49 by alubrano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	open_in_redir(t_redir *r, int *in_fd);
static int	open_out_redir(t_redir *r, int *out_fd, int flags);

/* Handle a single redirection node and open the target file. */
int	handle_single_redir(t_redir *r, int *in_fd, int *out_fd)
{
	if (r->type == R_IN || r->type == R_HEREDOC)
		return (open_in_redir(r, in_fd));
	if (r->type == R_OUT)
		return (open_out_redir(r, out_fd, O_WRONLY | O_CREAT | O_TRUNC));
	if (r->type == R_APPEND)
		return (open_out_redir(r, out_fd, O_WRONLY | O_CREAT | O_APPEND));
	return (0);
}

/* Open all redirections attached to a command AST node. */
int	open_redirections(t_ast *cmd, int *in_fd, int *out_fd)
{
	t_redir	*r;
	int		ret;

	if (!cmd || !cmd->redir)
		return (0);
	r = cmd->redir;
	while (r)
	{
		ret = handle_single_redir(r, in_fd, out_fd);
		if (ret == -1)
			return (-1);
		r = r->next;
	}
	return (0);
}

/* Apply initial fds and then open and apply redirections for a command. */
void	setup_fd(t_ast *cmd, int in_fd, int out_fd)
{
	if (in_fd >= 0)
		dup2(in_fd, STDIN_FILENO);
	if (out_fd >= 0)
		dup2(out_fd, STDOUT_FILENO);
	if (open_redirections(cmd, &in_fd, &out_fd) == -1)
		exit(1);
	if (in_fd >= 0)
	{
		dup2(in_fd, STDIN_FILENO);
		close(in_fd);
	}
	if (out_fd >= 0)
	{
		dup2(out_fd, STDOUT_FILENO);
		close(out_fd);
	}
}

/* Open input redirection (or heredoc) and update in_fd. */
static int	open_in_redir(t_redir *r, int *in_fd)
{
	if (r->fd < 0)
		return (-1);
	if (*in_fd >= 0)
		close(*in_fd);
	*in_fd = dup(r->fd);
	if (*in_fd < 0)
		return (perror("dup"), -1);
	return (0);
}

/* Open output redirection (truncate or append) and update out_fd. */
static int	open_out_redir(t_redir *r, int *out_fd, int flags)
{
	int	tmp;

	tmp = open(r->file, flags, 0777);
	if (tmp < 0)
		return (perror(r->file), -1);
	if (*out_fd >= 0)
		close(*out_fd);
	*out_fd = tmp;
	return (0);
}
