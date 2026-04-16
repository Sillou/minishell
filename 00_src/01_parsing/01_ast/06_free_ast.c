/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   06_free_ast.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcresce <dcresce@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 19:01:26 by dcresce           #+#    #+#             */
/*   Updated: 2026/04/08 19:01:45 by dcresce          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_redir(t_redir *redir)
{
	t_redir	*next;

	if (!redir)
		return ;
	while (redir)
	{
		next = redir->next;
		if (redir->fd >= 0)
			close(redir->fd);
		free(redir->file);
		free(redir);
		redir = next;
	}
}

void	free_ast(t_ast **ast)
{
	if (!ast)
		return ;
	free_ast_content(*ast);
	free(ast);
	ast = NULL;
}

void	free_ast_content(t_ast *ast)
{
	if (!ast)
		return ;
	free_ast_content(ast->left);
	free_ast_content(ast->right);
	if (ast->cmd)
		free_split(ast->cmd);
	if (ast->redir)
		free_redir(ast->redir);
	free(ast);
	ast = NULL;
}
