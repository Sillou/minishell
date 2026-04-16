/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   00_signal.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alubrano <alubrano@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/18 10:54:15 by alubrano          #+#    #+#             */
/*   Updated: 2026/04/04 21:56:14 by alubrano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <readline/readline.h>

/*
	install_sigint_handler(); LINE 25 26 dans main
	ignore_sigquit();

	lock_ctrl(); dans la boucle while juste avant gnl

	unlock_ctrl(); dans le exe_child pour exit;

	write_lock() dasn la avant la fonction pipe pour lock
	le retour decriture minishell

	pour le ctrl + D ce n'est pas un siganl c'est un EOF donc faut
	le gerer apres le gnl
		if (rd == -1)
		{
			write(1, "\nexit\n", 6);
			exit(ms->exit_code);
		}
*/

void	sigint_handler(int sig)
{
	(void)sig;
	if (g_ctrlc_locked == 1)
	{
		write(1, "\n", 1);
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
	else if (g_ctrlc_locked == 2)
	{
		write(1, "\n", 1);
		rl_on_new_line();
	}
	else if (g_ctrlc_locked == 3)
	{
		write(1, "^C\n", 3);
		rl_on_new_line();
	}
	else
		exit(130);
}

void	lock_ctrlc(void)
{
	g_ctrlc_locked = 1;
}

void	write_lock(void)
{
	g_ctrlc_locked = 2;
}

void	heredoc_lock(void)
{
	g_ctrlc_locked = 3;
}

void	unlock_ctrlc(void)
{
	g_ctrlc_locked = 0;
}
