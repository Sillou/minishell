/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   05_handle_redirections.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alubrano <alubrano@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/31 20:58:46 by dcresce           #+#    #+#             */
/*   Updated: 2026/04/08 11:21:35 by alubrano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	add_redir(t_redir_type type, char *file, int fd, t_redir **redir);

int	handle_infile(t_token *t, t_ast *ast)
{
	int		file_fd;
	char	*path;

	path = format_cmd(t->str, NULL, false);
	file_fd = open(path, O_RDONLY);
	if (file_fd < 0)
		return (free(path), perror("open"), 0);
	add_redir(R_IN, path, file_fd, &ast->redir);
	return (1);
}

int	handle_outfile(t_token *t, t_ast *ast)
{
	int		file_fd;
	char	*path;

	path = format_cmd(t->str, NULL, false);
	file_fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0777);
	if (file_fd < 0)
		return (free(path), perror("open"), 0);
	add_redir(R_OUT, path, file_fd, &ast->redir);
	return (1);
}

int	handle_append(t_token *t, t_ast *ast)
{
	int		file_fd;
	char	*path;

	path = format_cmd(t->str, NULL, false);
	file_fd = open(path, O_WRONLY | O_CREAT | O_APPEND, 0777);
	if (file_fd < 0)
		return (free(path), perror("open"), 0);
	add_redir(R_APPEND, path, file_fd, &ast->redir);
	return (1);
}

int	handle_heredoc(t_token *t, t_ast *ast, t_ms *ms)
{
	int		file_fd;
	char	*del;

	del = format_cmd(t->str, ms, false);
	file_fd = ft_here_doc(del, ms);
	if (file_fd < 0)
		return (free(del), 0);
	add_redir(R_HEREDOC, del, file_fd, &ast->redir);
	return (1);
}

static void	add_redir(t_redir_type type, char *file, int fd, t_redir **redir)
{
	t_redir	*new_redir;
	t_redir	*save;

	new_redir = ft_calloc(1, sizeof(t_redir));
	if (!new_redir)
		return ;
	new_redir->type = type;
	new_redir->file = file;
	new_redir->fd = fd;
	new_redir->next = NULL;
	if (!*redir)
	{
		*redir = new_redir;
		return ;
	}
	save = *redir;
	while ((*redir)->next)
		*redir = (*redir)->next;
	(*redir)->next = new_redir;
	*redir = save;
}
