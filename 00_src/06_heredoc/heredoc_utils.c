/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcresce <dcresce@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 19:28:31 by dcresce           #+#    #+#             */
/*   Updated: 2026/04/08 19:28:31 by dcresce          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	no_line(const char *de)
{
	char	*tmp;

	tmp = ft_strjoin(HEREDOC, de);
	msg_error("minishell", tmp, 0, NULL);
	free(tmp);
}

static const char	*find_tilde(const char *cmd)
{
	const char	*eq;

	if (*cmd == '~')
		return (cmd);
	eq = ft_strchr(cmd, '=');
	if (eq && eq[1] == '~')
		return (eq + 1);
	return (NULL);
}

char	*expand_tilde(const char *cmd, t_ms *ms)
{
	const char	*tilde;
	char		*home;
	char		*prefix;
	char		*result;

	tilde = find_tilde(cmd);
	if (!tilde || (tilde[1] != '/' && tilde[1] != '\0'))
		return (ft_strdup(cmd));
	home = get_var(*ms->envp, "HOME");
	if (!home)
		return (ft_strdup(cmd));
	prefix = ft_substr(cmd, 0, tilde - cmd);
	result = ft_strjoin(prefix, home);
	free(prefix);
	free(home);
	prefix = result;
	result = ft_strjoin(prefix, tilde + 1);
	free(prefix);
	return (result);
}

char	*expand_and_quotes(char *cmd, t_ms *ms)
{
	char	*form_cmd;

	if (ms && ft_strchr(cmd, '~') && !ft_strchr(cmd, '\'')
		&& !ft_strchr(cmd, '"'))
		form_cmd = expand_tilde(cmd, ms);
	else if ((ft_strchr(cmd, '\'')) || (ft_strchr(cmd, '"')))
		form_cmd = handle_quotes(ft_strdup(cmd), ms);
	else if (ft_strchr(cmd, '$'))
		form_cmd = handle_expand(cmd, ms);
	else
		form_cmd = ft_strdup(cmd);
	return (form_cmd);
}
