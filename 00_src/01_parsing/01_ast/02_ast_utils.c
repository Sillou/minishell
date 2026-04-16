/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   02_ast_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alubrano <alubrano@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/03 11:41:16 by dcresce           #+#    #+#             */
/*   Updated: 2026/04/07 20:17:28 by alubrano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	count_args(t_token *t)
{
	int	i;

	if (!t)
		return (0);
	i = 0;
	while (t && (t->type == ARG))
	{
		i++;
		t = t->prev;
		if (t && ft_isbetween(t->type, 7, 10))
			t = t->prev->prev;
	}
	return (i);
}

bool	is_builtin(char *cmd)
{
	if (ft_strncmp(cmd, "echo", 5) == 0)
		return (1);
	if (ft_strncmp(cmd, "cd", 3) == 0)
		return (1);
	if (ft_strncmp(cmd, "env", 4) == 0)
		return (1);
	if (ft_strncmp(cmd, "pwd", 4) == 0)
		return (1);
	if (ft_strncmp(cmd, "exit", 5) == 0)
		return (1);
	if (ft_strncmp(cmd, "export", 7) == 0)
		return (1);
	if (ft_strncmp(cmd, "unset", 6) == 0)
		return (1);
	else
		return (0);
}

t_ast	*subshell(t_token **t, t_ms *ms)
{
	t_ast	**subshell;
	t_ast	*first_node;
	int		first_op;

	subshell = ft_calloc(1, sizeof(*subshell));
	*t = (*t)->next;
	first_op = false;
	while (*t && ((*t)->type != P_OPEN))
	{
		if (!cmd_parser(t, subshell, ms, &first_op))
			return (free_ast(subshell), NULL);
	}
	first_node = new_ast(OP_SUB, 0, NULL);
	first_node->left = *subshell;
	free(subshell);
	if ((*t)->type == P_OPEN)
		*t = (*t)->next;
	return (first_node);
}

void	handle_cmd_add(t_token **t, t_ast *new_cmd, int *first_op, t_ast **ast)
{
	if ((*t)->type == P_OPEN)
	{
		if (*ast)
			last_head(*ast)->left = new_cmd->right;
		else
			*ast = new_cmd->right;
		free(new_cmd);
		return ;
	}
	new_cmd->op = get_op(*t);
	if (!(*first_op) && (new_cmd->op == OP_AND || new_cmd->op == OP_OR))
	{
		if (*ast)
		{
			last_head(*ast)->left = new_cmd->right;
			new_cmd->right = (*ast);
		}
		*ast = new_cmd;
		*first_op = true;
	}
	else
		add_cmd(ast, new_cmd);
	if ((*t)->type != P_OPEN)
		*t = (*t)->next;
}

int	handle_redir(t_token *t, t_ast *cmd, t_ms *ms, int n)
{
	int		r_c;
	int		saved_code;

	r_c = 1;
	saved_code = ms->exit_code;
	if (ft_isbetween(t->type, 7, 10))
		n++;
	while (t && t->prev && r_c && n--)
	{
		if (r_c && (t->type == HERE_DOC))
			r_c = handle_heredoc(t->prev, cmd, ms);
		if (r_c && t->type == APPEND)
			r_c = handle_append(t->prev, cmd);
		if (r_c && t->type == OUTPUT)
			r_c = handle_outfile(t->prev, cmd);
		if (r_c && t->type == INPUT)
			r_c = handle_infile(t->prev, cmd);
		t = t->prev;
		if (t->prev)
			t = t->prev;
	}
	if (r_c == 0 && ms->exit_code == saved_code)
		ms->exit_code = 126;
	return (r_c);
}
