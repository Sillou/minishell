/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   03_logical.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alubrano <alubrano@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/21 13:40:43 by alubrano          #+#    #+#             */
/*   Updated: 2026/04/05 00:11:34 by alubrano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	exec_and_node(t_ms *ms, t_ast *node)
{
	int	status;

	status = exec_ast(ms, node->left);
	if (status == -1)
		return (-1);
	ms->exit_code = status;
	if (status == 0)
	{
		status = exec_ast(ms, node->right);
		if (status != -1)
			ms->exit_code = status;
	}
	return (status);
}

int	exec_or_node(t_ms *ms, t_ast *node)
{
	int	status;

	status = exec_ast(ms, node->left);
	if (status == -1)
		return (-1);
	ms->exit_code = status;
	if (status != 0)
	{
		status = exec_ast(ms, node->right);
		if (status != -1)
			ms->exit_code = status;
	}
	return (status);
}
