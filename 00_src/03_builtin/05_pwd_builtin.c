/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd_builtin.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alubrano <alubrano@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 22:21:36 by alubrano          #+#    #+#             */
/*   Updated: 2026/02/28 14:32:27 by alubrano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
	here to execute pwd
	prints the current working directory
*/

int	builtin_pwd(t_ms *ms)
{
	char	*current;
	char	buffer[PATH_MAX];

	if (ms->c_dir)
	{
		ft_putendl_fd(ms->c_dir, 1);
		return (0);
	}
	current = getcwd(buffer, PATH_MAX);
	if (current)
	{
		ft_putendl_fd(current, 1);
		return (0);
	}
	ft_putendl_fd("pwd: error current directory", 2);
	return (1);
}

/*
	get current working directory
*/

char	*get_c_dir(void)
{
	char	buffer[PATH_MAX];
	char	*current;

	current = getcwd(buffer, PATH_MAX);
	if (current)
		return (ft_strdup(current));
	return (NULL);
}
