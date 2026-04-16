/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   alubrano.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alubrano <alubrano@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 21:14:16 by dcresce           #+#    #+#             */
/*   Updated: 2026/04/08 15:21:58 by alubrano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ALUBRANO_H
# define ALUBRANO_H

# include <limits.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/wait.h>
# include "structs.h"
# include <string.h>
# include <signal.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <dirent.h>

# define HEREDOC "here-document delimited by end-of-file wanted "
# define VAR "minishell : export : "
# define PATH "/usr/bin/"

typedef struct s_exe_cmd
{
	t_ms	*ms;
	t_ast	*node;
	int		in_fd;
	int		out_fd;
	int		in_pipe;
}	t_exe_cmd;

// esaie de voir pour la suppr
static volatile int	g_ctrlc_locked = 1;

/************************************/
/*				02_EXEC				*/
/************************************/

/*AST EXEC*/
int				exec_ast(t_ms *ms, t_ast *node);
int				exec_cmd_node(t_exe_cmd *execmd);
int				exec_pipe_node(t_ms *ms, t_ast *node);
int				exec_and_node(t_ms *ms, t_ast *node);
int				exec_or_node(t_ms *ms, t_ast *node);
int				exec_sub_node(t_ms *ms, t_ast *node);

/*EXEC UTILS*/
int				exec_parent_builtin(t_exe_cmd *execmd);
int				run_builtin_on_node(t_exe_cmd *execmd);
int				must_run_builtin_in_parent(t_exe_cmd *execmd);
int				is_parent_builtin(t_ast *node);

/*FD UTILS*/
void			ft_execut(t_ms *ms, t_ast *cmd);
void			ft_get_path(t_ms *ms);
void			setup_fd(t_ast *cmd, int in_fd, int out_fd);
void			close_fd(int fd);
int				wait_one_child(pid_t pid);

/*WILDCARD*/
int				wc_match(const char *pattern, const char *str);
char			**expand_wildcard(const char *pattern);
void			free_wildcard_matches(char **matches);
char			**expand_wildcards_in_cmd(char **cmd);

/************************************/
/*				03_BUILTIN			*/
/************************************/

/*CD*/
int				builtin_cd(t_ms *ms);

/*ECHO*/
int				builtin_echo(t_ms *ms);
void			print_with_expansion(t_ms *ms, char *s, int i);

/*ECHO_UTILS*/
void			print_single_quoted(const char *s);
char			*strip_double_quotes(const char *s);
void			check_dollar(t_ms *ms, char *s, int *j);

/*ENV*/
int				builtin_env(t_ms *ms);
void			add_shlvl(t_ms *ms);
int				count_env(t_envp *envp);
void			if_no_env(t_ms *ms);
char			**envp_to_array(t_envp *envp);
void			free_env_array(char **envp);
void			free_export(char **tmp);
int				count_tmp(char **tmp);

/*EXEC*/
int				builtin_exec(t_ms *ms);

/*EXIT*/
int				builtin_exit(t_ms *ms);
void			ft_exit(t_ms *ms);
void			ft_free(t_ms *ms);
void			free_cmd(t_ast *n);
void			free_redire(t_redir *r);

/*EXPORT*/
int				builtin_export(t_ms *ms);
void			new_set_envp(t_ms *ms, char	*var, char	*value);
char			*tmp_copy(t_envp *node);
int				export_sort(t_ms *ms);
void			sortin_tmp(char **tmp, int k);
void			print_export(char **tmp);
int				ft_strcmp(const char *s1, const char *s2);

/*PWD*/
int				builtin_pwd(t_ms *ms);
char			*get_c_dir(void);

/*UNSET*/
int				builtin_unset(t_ms *ms, char **args);
t_envp			*check_exist(t_envp *envp, char *var);

/*UTILS*/
int				msg_error(char *str, char *error_msg, int nb_error, t_ms *ms);
int				is_valid(char *arg);

/************************************/
/*				04_SIGNAL			*/
/************************************/

/*SIGNAL*/
void			sigint_handler(int sig);
void			lock_ctrlc(void);
void			write_lock(void);
void			heredoc_lock(void);
void			unlock_ctrlc(void);
void			ignore_sigquit(void);
void			install_sigint_handler(void);

/************************************/
/*				06_HEREDOC			*/
/************************************/

/*HEREDOC*/
int				ft_here_doc(char *del, t_ms *ms);
void			no_line(const char *de);
char			*expand_tilde(const char *cmd, t_ms *ms);

#endif
