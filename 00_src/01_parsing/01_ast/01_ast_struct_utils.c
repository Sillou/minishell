/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   01_ast_struct_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcresce <dcresce@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 18:30:03 by dcresce           #+#    #+#             */
/*   Updated: 2026/03/24 18:30:03 by dcresce          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_ast	*new_ast(t_op op, bool built_in, char **cmd)
{
	t_ast	*ast;

	ast = ft_calloc(1, sizeof(t_ast));
	if (!ast)
		return (NULL);
	ast->left = NULL;
	ast->right = NULL;
	ast->op = op;
	ast->redir = NULL;
	ast->built_in = built_in;
	ast->cmd = cmd;
	return (ast);
}

void	add_cmd(t_ast **ast, t_ast *new)
{
	if (!ast | !new)
		return ;
	if (!*ast)
	{
		*ast = new;
		return ;
	}
	last_head(*ast)->left = new;
}

t_ast	*last_head(t_ast *ast)
{
	if (!ast)
		return (NULL);
	while (ast->left)
		ast = ast->left;
	return (ast);
}
