/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   08_wc_match_check.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alubrano <alubrano@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/02 23:02:27 by alubrano          #+#    #+#             */
/*   Updated: 2026/04/03 11:10:55 by alubrano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	match_char(const char **p, const char **s);
static void	save_star(const char **star, const char **match,
				const char **p, const char **s);
static void	backtrack_star(const char **p, const char **s,
				const char **star, const char **match);

int	wc_match(const char *pattern, const char *str)
{
	const char	*p;
	const char	*s;
	const char	*star;
	const char	*match;

	if (!pattern || !str || (str[0] == '.' && pattern[0] != '.'))
		return (0);
	p = pattern;
	s = str;
	star = NULL;
	match = NULL;
	while (*s)
	{
		if (*p == *s)
			match_char(&p, &s);
		else if (*p == '*')
			save_star(&star, &match, &p, &s);
		else if (star)
			backtrack_star(&p, &s, &star, &match);
		else
			return (0);
	}
	while (*p == '*')
		p++;
	return (*p == '\0');
}

static void	match_char(const char **p, const char **s)
{
	(*p)++;
	(*s)++;
}

static void	save_star(const char **star, const char **match,
		const char **p, const char **s)
{
	*star = *p;
	(*p)++;
	*match = *s;
}

static void	backtrack_star(const char **p, const char **s,
		const char **star, const char **match)
{
	*p = *star + 1;
	*match = *match + 1;
	*s = *match;
}

void	free_wildcard_matches(char **matches)
{
	int	i;

	if (!matches)
		return ;
	i = 0;
	while (matches[i])
		free(matches[i++]);
	free(matches);
}
