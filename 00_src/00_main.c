/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   00_main.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alubrano <alubrano@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/19 15:36:44 by dcresce           #+#    #+#             */
/*   Updated: 2026/04/05 00:07:33 by alubrano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <readline/readline.h>
#include <readline/history.h>

static int	handle_line(t_ms **ms, char *line);
static t_ms	**init_ms(char **envp);

int	main(int argc, char **argv, char **envp)
{
	t_ms	**ms;
	char	*line;
	int		exit_code;

	(void)argc;
	(void)argv;
	ms = init_ms(envp);
	if (!ms)
		return (1);
	while (1)
	{
		lock_ctrlc();
		line = readline("minishell > ");
		if (!line)
			break ;
		if (*line)
			add_history(line);
		if (handle_line(ms, line) == -1)
			break ;
		unlock_ctrlc();
	}
	exit_code = (*ms)->exit_code;
	ft_exit(*ms);
	free(ms);
	return (exit_code);
}

static t_ms	**init_ms(char **envp)
{
	t_ms	**ms;

	install_sigint_handler();
	ignore_sigquit();
	ms = pars(NULL, envp);
	if (!ms || !*ms)
		return (NULL);
	if (!*(*ms)->envp)
	{
		if_no_env(*ms);
		new_set_envp(*ms, "SHLVL", ft_itoa(0));
	}
	add_shlvl(*ms);
	return (ms);
}

static int	handle_line(t_ms **ms, char *line)
{
	t_ast	**tree;
	int		saved_code;
	int		res;

	tree = NULL;
	if (!line || !*line)
		return (free(line), 0);
	saved_code = (*ms)->exit_code;
	tree = ast(line, ms);
	if (tree && *tree)
	{
		(*ms)->cmds = tree;
		res = exec_ast(*ms, *tree);
		free_ast(tree);
		(*ms)->cmds = NULL;
		if (res == -1)
			return (free(line), -1);
		(*ms)->exit_code = res;
	}
	else if (tree)
		free(tree);
	else if ((*ms)->exit_code == saved_code)
		(*ms)->exit_code = 2;
	free(line);
	return (0);
}
