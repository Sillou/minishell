/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   00_token.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcresce <dcresce@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 19:14:16 by dcresce           #+#    #+#             */
/*   Updated: 2026/04/08 19:14:19 by dcresce          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	add_token(char *str, int *start, int *end, t_token **t);
static void	handle_sp_char(char *s, int *st, int *end, t_token **t);
static void	t_handle_quotes(char *str, int *start, int *end);
static int	special_commands(char *piece, t_token **t);

t_token	**tokens(char *s)
{
	int		st;
	int		end;
	t_token	**t;

	st = 0;
	t = ft_calloc(1, sizeof(*t));
	while (s[st] && !errno)
	{
		while (!errno && s[st] && (s[st] == ' '))
			st++;
		end = st;
		if (!errno && s[st] && ft_strchr("\"'&|()<>", s[st]))
			handle_sp_char(s, &st, &end, t);
		while (!errno && s[end] && !ft_strchr("&|()<> ", s[end]))
		{
			if ((s[end] == '"') || (s[end] == '\''))
				t_handle_quotes(s, &st, &end);
			else
				end++;
		}
		add_token(s, &st, &end, t);
		if (errno)
			return (free_tokens(t), errno = 0, NULL);
	}
	return (t);
}

static void	handle_sp_char(char *s, int *st, int *end, t_token **t)
{
	if ((s[*st] == '"') || (s[*st] == '\''))
	{
		t_handle_quotes(s, st, end);
		add_token(s, st, end, t);
		return ;
	}
	if (s[*st] == '(')
	{
		new_token(ft_strdup("("), P_OPEN, t);
		(*st)++;
		*end = *st;
		return ;
	}
	if (s[*st] == ')')
	{
		new_token(ft_strdup(")"), P_CLOSE, t);
		(*st)++;
		*end = *st;
		return ;
	}
	*end = *st;
	while (s[*st] == s[*end])
		(*end)++;
	add_token(s, st, end, t);
}

static void	t_handle_quotes(char *str, int *start, int *end)
{
	char	check;

	if (*start > *end)
	{
		check = str[*start];
		*end = *start + 1;
	}
	else
	{
		check = str[*end];
		(*end)++;
	}
	while (str[*end] && (str[*end] != check))
		(*end)++;
	(*end)++;
}

static void	add_token(char *str, int *start, int *end, t_token **t)
{
	char	*piece;
	int		type;

	if (*start == *end)
		return ;
	piece = ft_substr(str, (unsigned int)*start, *end - *start);
	if (!piece)
		return (perror("malloc"));
	if ((!special_commands(piece, t)) && *t)
	{
		type = (*t)->type;
		if ((type == ARG) && ft_isbetween((*t)->next->type, 7, 10)
			&& !cmd_before_op(*t))
			new_token(piece, CMD, t);
		else if (ft_isbetween(type, 7, 10))
			new_token(piece, ARG, t);
		else if ((type != CMD) && (type != ARG))
			new_token(piece, CMD, t);
		else
			new_token(piece, ARG, t);
	}
	else if (!*t)
		new_token(piece, CMD, t);
	*start = *end;
	near_op(*t);
}

static int	special_commands(char *piece, t_token **t)
{
	if (!ft_strncmp(piece, "|", 2))
		new_token(piece, PIPE, t);
	else if (!ft_strncmp(piece, "<", 2))
		new_token(piece, INPUT, t);
	else if (!ft_strncmp(piece, ">", 2))
		new_token(piece, OUTPUT, t);
	else if (!ft_strncmp(piece, "&&", 3))
		new_token(piece, AND, t);
	else if (!ft_strncmp(piece, "||", 3))
		new_token(piece, OR, t);
	else if (!ft_strncmp(piece, "<<", 3))
		new_token(piece, HERE_DOC, t);
	else if (!ft_strncmp(piece, ">>", 3))
		new_token(piece, APPEND, t);
	else
		return (0);
	return (1);
}
