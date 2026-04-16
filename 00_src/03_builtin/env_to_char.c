/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_to_char.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alubrano <alubrano@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 13:58:07 by alubrano          #+#    #+#             */
/*   Updated: 2026/03/09 13:55:27 by alubrano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*envp_copy(t_envp *node);

char	**envp_to_array(t_envp *envp)
{
	char	**arr;
	int		count;
	int		i;

	count = count_env(envp);
	arr = malloc(sizeof(char *) * (count + 1));
	if (!arr)
		return (NULL);
	i = 0;
	arr[0] = NULL;
	while (envp)
	{
		arr[i] = envp_copy(envp);
		if (!arr[i])
		{
			while (i > 0)
				free(arr[--i]);
			free(arr);
			return (NULL);
		}
		envp = envp->next;
		i++;
	}
	arr[i] = NULL;
	return (arr);
}

void	free_env_array(char **envp)
{
	int	i;

	if (!envp)
		return ;
	i = 0;
	while (envp[i])
		free(envp[i++]);
	free(envp);
}

void	free_export(char **tmp)
{
	int	i;

	i = 0;
	while (tmp[i])
		free(tmp[i++]);
	free(tmp);
}

int	count_tmp(char **tmp)
{
	int	i;

	i = 0;
	while (tmp[i])
		i++;
	return (i);
}

static char	*envp_copy(t_envp *node)
{
	char	*tmp;
	char	*line;

	if (!node || !node->var)
		return (NULL);
	if (!node->content)
		return (ft_strdup(node->var));
	tmp = ft_strjoin(node->var, "=");
	if (!tmp)
		return (NULL);
	line = ft_strjoin(tmp, node->content);
	free(tmp);
	return (line);
}
