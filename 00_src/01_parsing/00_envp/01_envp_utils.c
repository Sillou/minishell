/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   01_envp_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcresce <dcresce@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 19:15:26 by dcresce           #+#    #+#             */
/*   Updated: 2026/03/10 19:15:26 by dcresce          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// creates a new variable
// arguments: const char s
// It takes the string and cuts at the first '=', 
// saving the first part in var and the rest in content
t_envp	*new_var(const char *s)
{
	t_envp		*n;
	const char	*eq;

	n = malloc(sizeof(*n));
	if (!n)
		return (NULL);
	eq = ft_strchr(s, '=');
	if (eq)
	{
		n->var = ft_substr(s, 0, eq - s);
		n->content = ft_strdup(eq + 1);
	}
	else
	{
		n->var = ft_strdup(s);
		n->content = NULL;
	}
	n->next = NULL;
	if (!n->var || (eq && !n->content))
		return (free(n->var), free(n->content), free(n), NULL);
	return (n);
}

//gets the last variable in the envp
t_envp	*last_var(t_envp *envp)
{
	if (!envp)
		return (0);
	while (envp->next)
		envp = envp->next;
	return (envp);
}

//add a new variable to the envp
int	add_var(t_envp **envp, t_envp *new)
{
	if (!envp || !new)
		return (0);
	if (*envp)
		last_var(*envp)->next = new;
	else
		*envp = new;
	return (1);
}
