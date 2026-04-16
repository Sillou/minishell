/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   03_cd_builtin.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alubrano <alubrano@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/08 16:02:33 by alubrano          #+#    #+#             */
/*   Updated: 2026/04/05 20:28:25 by alubrano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
	change the directory
	update OLPWD and new one in PATH
*/

static char	*check_dir(t_envp *envp, char *var);
static char	*get_cd_path(t_ms *ms);
static int	cd_error_no_path(char *arg);

int	builtin_cd(t_ms *ms)
{
	char	*path;
	char	*new;
	char	*old;

	if (!ms->cmds || !*ms->cmds || !(*ms->cmds)->cmd)
		return (1);
	if ((*ms->cmds)->cmd[1] && (*ms->cmds)->cmd[2])
		return (ft_putendl_fd("minishell: cd: too many arguments", 2), 1);
	path = get_cd_path(ms);
	if (!path)
		return (cd_error_no_path((*ms->cmds)->cmd[1]));
	old = get_c_dir();
	if (chdir(path) == -1)
		return (perror("cd"), free(old), 1);
	new = get_c_dir();
	if ((*ms->cmds)->cmd[1]
		&& ft_strncmp((*ms->cmds)->cmd[1], "-", 2) == 0)
		ft_putendl_fd(new, 1);
	new_set_envp(ms, "OLDPWD", old);
	new_set_envp(ms, "PWD", new);
	free(new);
	return (free(old), 0);
}

static char	*check_dir(t_envp *envp, char *var)
{
	if (!var || !envp)
		return (NULL);
	while (envp)
	{
		if (envp->var && ft_strncmp(envp->var, var, ft_strlen(var) + 1) == 0)
			return (envp->content);
		envp = envp->next;
	}
	return (NULL);
}

static char	*get_cd_path(t_ms *ms)
{
	char	*arg;

	arg = (*ms->cmds)->cmd[1];
	if (!arg
		|| ft_strncmp(arg, "~", 2) == 0
		|| ft_strncmp(arg, "--", 3) == 0)
		return (check_dir(*ms->envp, "HOME"));
	if (ft_strncmp(arg, "-", 2) == 0)
		return (check_dir(*ms->envp, "OLDPWD"));
	return (arg);
}

static int	cd_error_no_path(char *arg)
{
	if (arg && ft_strncmp(arg, "-", 2) == 0)
		ft_putendl_fd("minishell: cd: OLDPWD not set", 2);
	else
		ft_putendl_fd("minishell: cd: HOME not set", 2);
	return (1);
}
