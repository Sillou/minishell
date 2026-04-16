/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   01_token_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcresce <dcresce@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 19:11:36 by dcresce           #+#    #+#             */
/*   Updated: 2026/04/08 19:11:42 by dcresce          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	new_token(char *str, int type, t_token **t)
{
	t_token	*token;

	token = (t_token *)malloc(sizeof(t_token));
	if (!token)
		return (perror("malloc"));
	token->str = str;
	token->type = type;
	token->prev = NULL;
	if (!*t)
	{
		*t = token;
		token->next = NULL;
		return ;
	}
	token->next = *t;
	(*t)->prev = token;
	*t = token;
}

t_token	*last_token(t_token *t)
{
	if (!t)
		return (NULL);
	while (t->next)
		t = t->next;
	return (t);
}

int	cmd_before_op(t_token *t)
{
	if (!t)
		return (0);
	while (t && ((t->type != PIPE) && (t->type != AND) && (t->type != OR)))
	{
		if (t->type == CMD)
			return (1);
		t = t->next;
	}
	return (0);
}

void	near_op(t_token *t)
{
	if (!t || !t->next)
		return ;
	if ((t->type == PIPE) || (t->type == AND) || (t->type == OR))
	{
		t = t->next;
		if ((t->type == PIPE) || ((t->type >= AND) && (t->type <= OUTPUT)))
		{
			msg_error(SYN_ERR, t->str, 0, NULL);
			errno = 1;
		}
	}
}

t_op	get_op(t_token *t)
{
	if (t->type == AND)
		return (OP_AND);
	if (t->type == OR)
		return (OP_OR);
	if (t->type == PIPE)
		return (OP_PIPE);
	return (OP_NONE);
}
