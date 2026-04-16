/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   03_format_cmd.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcresce <dcresce@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 19:22:02 by dcresce           #+#    #+#             */
/*   Updated: 2026/04/08 19:27:33 by dcresce          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*check_path(char *cmd, t_envp *envp);

char	*format_cmd(char *cmd, t_ms *ms, bool is_cmd)
{
	char	*form_cmd;
	char	*temp;

	if (!*cmd)
		return (NULL);
	form_cmd = expand_and_quotes(cmd, ms);
	if (is_cmd)
	{
		if (ft_strchr(form_cmd, '/'))
			return (form_cmd);
		if (is_builtin(form_cmd))
			return (form_cmd);
		temp = check_path(form_cmd, *ms->envp);
		if (temp != form_cmd)
		{
			free(form_cmd);
			form_cmd = temp;
		}
	}
	return (form_cmd);
}

static char	*check_path(char *cmd, t_envp *envp)
{
	int		i;
	char	*temp;
	char	*candidate;
	char	**path;

	while (envp && ft_strncmp(envp->var, "PATH", 4))
		envp = envp->next;
	if (!envp || !envp->content)
		return (cmd);
	path = ft_split(envp->content, ':');
	i = -1;
	while (path[++i])
	{
		temp = ft_strjoin(path[i], "/");
		if (!temp)
			return (perror("malloc"), NULL);
		candidate = ft_strjoin(temp, cmd);
		if (!candidate)
			return (perror("malloc"), NULL);
		if (!access(candidate, X_OK))
			return (free_split(path), free(temp), candidate);
		free (temp);
		free (candidate);
	}
	return (free_split(path), cmd);
}

char	*handle_expand(char *s, t_ms *ms)
{
	char	*out;
	char	*temp;
	char	*sub;
	int		i;
	int		j;

	temp = get_expanded(s, ms, &i, &j);
	j++;
	sub = ft_substr(s, 0, i);
	out = ft_strjoin(sub, temp);
	free(temp);
	free(sub);
	sub = ft_substr(s, j, (ft_strlen(s) - j));
	if (sub && ft_strchr(sub, '$'))
	{
		temp = handle_expand(sub, ms);
		free(sub);
		sub = temp;
	}
	temp = ft_strjoin(out, sub);
	return (free(out), free(sub), temp);
}

char	*expand_helper(char *s, t_ms *ms, int *i, int *j)
{
	char	*sub;
	bool	flag;
	char	*var;

	if (s[*j] && !ft_isalnum(s[*j]) && s[*j] != '_')
	{
		*j = *i;
		return (ft_strdup(""));
	}
	flag = false;
	while (s[*j] && (s[*j] != ' ') && (s[*j] != '"')
		&& (s[*j] != '\'') && (s[*j] != '\n'))
	{
		if (!ft_isalnum(s[*j]) && (s[*j] != '_'))
			flag = true;
		(*j)++;
	}
	sub = ft_substr(s, *i + 1, *j - *i - 1);
	var = get_var(*ms->envp, sub);
	(*j)--;
	if (flag)
		return (free(var), sub);
	else
		return (free(sub), var);
}

char	*get_var(t_envp *envp, char *var)
{
	while (envp)
	{
		if (envp->var && ft_strncmp(envp->var, var, ft_strlen(var) + 1) == 0)
			return (ft_strdup(envp->content));
		envp = envp->next;
	}
	return (NULL);
}
