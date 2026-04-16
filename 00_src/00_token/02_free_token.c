/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   02_free_token.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcresce <dcresce@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 19:10:04 by dcresce           #+#    #+#             */
/*   Updated: 2026/04/08 19:11:17 by dcresce          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	del_token(t_token *t)
{
	if (!t)
		return ;
	free(t->str);
	free(t);
}

void	free_tokens(t_token **t)
{
	t_token	*next;

	if (!t)
		return ;
	if (!*t)
	{
		free(t);
		return ;
	}
	while (*t)
	{
		next = (*t)->next;
		del_token(*t);
		*t = next;
	}
	free(t);
	t = NULL;
}

void	token_error(t_token **t, char *err)
{
	msg_error(err, NULL, 0, NULL);
	if (t)
		free_tokens(t);
}
