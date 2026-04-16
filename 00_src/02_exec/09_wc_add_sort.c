/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   09_wc_add_sort.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alubrano <alubrano@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 21:47:06 by alubrano          #+#    #+#             */
/*   Updated: 2026/04/03 11:11:45 by alubrano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	**free_matches(char **tab);
static char	**add_match(char **old, int count, char *name);
static void	sort_matches(char **tab);
static void	collect_matches(DIR *dir, const char *pattern,
				char ***matches, int *count);

char	**expand_wildcard(const char *pattern)
{
	DIR		*dir;
	char	**matches;
	int		count;

	if (!pattern || (!ft_strchr(pattern, '*')))
		return (NULL);
	dir = opendir(".");
	if (!dir)
		return (NULL);
	matches = NULL;
	count = 0;
	collect_matches(dir, pattern, &matches, &count);
	closedir(dir);
	if (!matches)
		return (NULL);
	sort_matches(matches);
	return (matches);
}

static char	**free_matches(char **tab)
{
	int	i;

	if (!tab)
		return (NULL);
	i = 0;
	while (tab[i])
	{
		free(tab[i]);
		i++;
	}
	free(tab);
	return (NULL);
}

static char	**add_match(char **old, int count, char *name)
{
	char	**new_tab;
	int		i;

	new_tab = ft_calloc(count + 2, sizeof(char *));
	if (!new_tab)
		return (free_matches(old));
	i = 0;
	while (i < count)
	{
		new_tab[i] = old[i];
		i++;
	}
	new_tab[i] = ft_strdup(name);
	if (!new_tab[i])
	{
		free(new_tab);
		return (free_matches(old));
	}
	free(old);
	return (new_tab);
}

static void	sort_matches(char **tab)
{
	int		i;
	int		j;
	size_t	max;
	char	*tmp;

	i = 0;
	while (tab && tab[i])
	{
		j = i + 1;
		while (tab[j])
		{
			max = ft_strlen(tab[i]);
			if (ft_strlen(tab[j]) > max)
				max = ft_strlen(tab[j]);
			if (ft_strncmp(tab[i], tab[j], max + 1) > 0)
			{
				tmp = tab[i];
				tab[i] = tab[j];
				tab[j] = tmp;
			}
			j++;
		}
		i++;
	}
}

static void	collect_matches(DIR *dir, const char *pattern,
		char ***matches, int *count)
{
	struct dirent	*entry;

	entry = readdir(dir);
	while (entry)
	{
		if (wc_match(pattern, entry->d_name))
			*matches = add_match(*matches, (*count)++, entry->d_name);
		if (!*matches && *count > 0)
			break ;
		entry = readdir(dir);
	}
}
