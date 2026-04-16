/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   01_echo_builtin.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alubrano <alubrano@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 18:47:13 by alubrano          #+#    #+#             */
/*   Updated: 2026/04/05 00:35:04 by alubrano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static bool	is_flag(char *args);
static void	echo_print(t_ms *ms, char **cmd, bool n_flag, int i);
static bool	is_single_quoted(const char *s);

/*
	main fct for echo
	check if there is flag
	then print args
*/

int	builtin_echo(t_ms *ms)
{
	bool	n_flag;
	int		i;

	if (!ms || !ms->cmds || !*ms->cmds || !(*ms->cmds)->cmd)
		return (1);
	n_flag = false;
	i = 1;
	while ((*ms->cmds)->cmd[i] && is_flag((*ms->cmds)->cmd[i]))
	{
		n_flag = true;
		i++;
	}
	echo_print(ms, (*ms->cmds)->cmd, n_flag, i);
	return (0);
}

/*
	check flag multipole n
*/

static bool	is_flag(char *args)
{
	int	i;

	if (args && !ft_strncmp(args, "\"-n\"", 5))
		return (true);
	if (!args || args[0] != '-' || args[1] == '\0')
		return (false);
	i = 1;
	while (args[i])
	{
		if (args[i] != 'n')
			return (false);
		i++;
	}
	return (true);
}

/*
	print args add newline if flag is false
*/

static void	echo_print(t_ms *ms, char **cmd, bool n_flag, int i)
{
	int	first;

	(void)ms;
	first = 1;
	while (cmd[i])
	{
		if (!first)
			ft_putchar_fd(' ', 1);
		ft_putstr_fd(cmd[i], 1);
		first = 0;
		i++;
	}
	if (!n_flag)
		ft_putchar_fd('\n', 1);
}

void	print_with_expansion(t_ms *ms, char *s, int i)
{
	char	*work;

	if (!s)
		return ;
	if (is_single_quoted(s))
	{
		print_single_quoted(s);
		return ;
	}
	work = strip_double_quotes(s);
	if (work)
		s = work;
	while (s[i])
	{
		if (s[i] == '$')
		{
			check_dollar(ms, s, &i);
			continue ;
		}
		ft_putchar_fd(s[i], 1);
		i++;
	}
	if (work)
		free(work);
}

static bool	is_single_quoted(const char *s)
{
	size_t	l;

	if (!s || s[0] != '\'')
		return (false);
	l = ft_strlen(s);
	return (l >= 2 && s[l - 1] == '\'');
}
