/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   02_envp_clean.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcresce <dcresce@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 19:15:15 by dcresce           #+#    #+#             */
/*   Updated: 2026/03/10 19:15:15 by dcresce          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

//remove one variable from the var
void	del_var(t_envp *var)
{
	if (!var)
		return ;
	free(var->var);
	free(var->content);
	free(var);
}

//clear the envp
void	clear_envp(t_envp **envp)
{
	t_envp	*next;

	if (!envp)
		return ;
	while (*envp)
	{
		next = (*envp)->next;
		del_var(*envp);
		*envp = next;
	}
	free(envp);
}
