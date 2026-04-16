/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   08_export_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alubrano <alubrano@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 14:14:39 by alubrano          #+#    #+#             */
/*   Updated: 2026/03/31 11:43:32 by alubrano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*tmp_copy(t_envp *node)
{
	char	*tmp;
	char	*line;

	if (!node || !node->var)
		return (NULL);
	if (!node->content)
		return (ft_strdup(node->var));
	tmp = ft_strjoin(node->var, "=\"");
	if (!tmp)
		return (NULL);
	line = ft_strjoin(tmp, node->content);
	tmp = ft_strjoin(line, "\"");
	free(line);
	return (tmp);
}

int	export_sort(t_ms *ms)
{
	int		i;
	char	**tmp;
	t_envp	*current;

	i = 0;
	if (!ms || !*ms->envp)
		return (1);
	tmp = malloc(sizeof(char *) * (count_env(*ms->envp) + 1));
	if (!tmp)
		return (1);
	current = *ms->envp;
	while (current)
	{
		tmp[i] = tmp_copy(current);
		if (!tmp[i])
			return (free_export(tmp), 1);
		current = current->next;
		i++;
	}
	tmp[i] = NULL;
	sortin_tmp(tmp, i);
	free_export(tmp);
	return (0);
}

void	sortin_tmp(char **tmp, int k)
{
	char	*swap;
	int		i;
	int		j;

	j = 0;
	while (tmp && j == 0 && k > 0)
	{
		j = 1;
		i = 0;
		while (i < k && tmp[i + 1] != NULL)
		{
			if (ft_strcmp(tmp[i], tmp[i + 1]) > 0)
			{
				swap = tmp[i];
				tmp[i] = tmp[i + 1];
				tmp[i + 1] = swap;
				j = 0;
			}
			i++;
		}
		k--;
	}
	print_export(tmp);
}

void	print_export(char **tmp)
{
	int	i;

	i = 0;
	while (tmp[i])
	{
		if (count_tmp(tmp) < 10)
		{
			if (!ft_strncmp(tmp[i], "PATH", 4))
				i++;
		}
		printf("declare -x %s \n", tmp[i]);
		i++;
	}
}

int	ft_strcmp(const char *s1, const char *s2)
{
	size_t	i;

	i = 0;
	while (s1[i] == s2[i])
	{
		if (s1[i] == '\0' && s2[i] == '\0')
			return (0);
		i++;
	}
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}
