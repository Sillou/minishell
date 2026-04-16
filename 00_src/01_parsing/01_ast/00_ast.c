/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   00_ast.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alubrano <alubrano@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/03 08:53:21 by dcresce           #+#    #+#             */
/*   Updated: 2026/04/07 20:41:55 by alubrano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	create_cmd(t_token **t, t_ast *cmd, t_ms *ms, int *r_c);
static int	handle_cmd(t_token **t, t_ast *ast, t_ms *ms);
static void	fill_cmd(char **cmd, t_token *t, t_ms *ms);

t_ast	**ast(char *str, t_ms **ms)
{
	t_token	**t;
	t_ast	**ast;
	t_token	*save;
	char	*line_form;
	int		first_op;

	line_form = check_cmd(str);
	if (!line_form)
		return (NULL);
	t = tokens(line_form);
	if (line_form != str)
		free(line_form);
	if (!t)
		return (NULL);
	ast = ft_calloc(1, sizeof(*ast));
	save = *t;
	first_op = false;
	while (*t)
	{
		if (!cmd_parser(t, ast, *ms, &first_op))
			return (free_ast(ast), *t = save, free_tokens(t), NULL);
	}
	*t = save;
	free_tokens(t);
	return (ast);
}

int	cmd_parser(t_token **t, t_ast **ast, t_ms *ms, int *first_op)
{
	int			r_c;
	t_ast		*new_cmd;

	r_c = 1;
	new_cmd = new_ast(OP_NONE, 0, NULL);
	if ((*t)->type == P_CLOSE)
		new_cmd->right = subshell(t, ms);
	else
	{
		new_cmd->right = new_ast(OP_NONE, 0, NULL);
		create_cmd(t, new_cmd->right, ms, &r_c);
	}
	if (!r_c)
		return (free_ast_content(new_cmd), 0);
	if (*t)
	{
		handle_cmd_add(t, new_cmd, first_op, ast);
		return (r_c);
	}
	else if (!*t && !*ast)
		add_cmd(ast, new_cmd->right);
	else
		last_head(*ast)->left = new_cmd->right;
	return (free(new_cmd), r_c);
}

static void	create_cmd(t_token **t, t_ast *cmd, t_ms *ms, int *r_c)
{
	int	type;
	int	count;

	while (*r_c && *t && !ft_isbetween((*t)->type, PIPE, OR))
	{
		type = (*t)->type;
		if (*r_c && *t && type == CMD)
			*r_c = handle_cmd(t, cmd, ms);
		if (*r_c && *t && (type >= HERE_DOC && type <= OUTPUT))
		{
			count = 0;
			while (*t && (*t)->next)
			{
				if (((*t)->type >= HERE_DOC) && ((*t)->type <= OUTPUT))
					count++;
				if ((!(*t)->next || !(*t)->next->next) || ((*t)->next->next
						&& !ft_isbetween((*t)->next->next->type, 7, 10)))
					break ;
				*t = (*t)->next->next;
			}
			*r_c = handle_redir(*t, cmd, ms, count);
		}
		if (*t)
			*t = (*t)->next;
	}
}

static int	handle_cmd(t_token **t, t_ast *ast, t_ms *ms)
{
	char	**cmd;

	cmd = ft_calloc(count_args((*t)->prev) + 2, sizeof(char *));
	if (!cmd)
		return (perror("malloc"), 0);
	fill_cmd(cmd, *t, ms);
	(ast)->cmd = cmd;
	(ast)->built_in = is_builtin(cmd[0]);
	return (1);
}

static void	fill_cmd(char **cmd, t_token *t, t_ms *ms)
{
	int	i;

	(void)ms;
	if (!t)
		return ;
	cmd[0] = ft_strdup(t->str);
	t = t->prev;
	i = 1;
	while (t && (t->type == ARG))
	{
		cmd[i++] = ft_strdup(t->str);
		t = t->prev;
		if (t && ft_isbetween(t->type, 7, 10))
			t = t->prev->prev;
	}
	cmd[i] = NULL;
}
