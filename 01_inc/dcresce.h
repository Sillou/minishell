/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dcresce.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcresce <dcresce@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 19:21:44 by dcresce           #+#    #+#             */
/*   Updated: 2026/04/08 19:21:56 by dcresce          ###   ########.ch       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DCRESCE_H
# define DCRESCE_H

# include <sys/wait.h>
# include <stdio.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <errno.h>

# ifndef BONUS
#  define BONUS 1
# endif

//define types
# define CMD		0
# define ARG		1
# define PIPE		2
# define P_OPEN		3
# define P_CLOSE	4
# define AND		5
# define OR			6
# define HERE_DOC	7
# define APPEND		8
# define INPUT		9
# define OUTPUT		10

//define errors
# define SYN_ERR	"minishell: syntax error near unexpected token"

typedef struct s_token
{
	char			*str;
	int				type;
	struct s_token	*next;
	struct s_token	*prev;
}	t_token;

//tokens
t_token	**tokens(char *input);
void	new_token(char *str, int type, t_token **t);
void	free_tokens(t_token **t);
void	token_error(t_token **t, char *err);
t_token	*last_token(t_token *t);
int		cmd_before_op(t_token *t);
void	near_op(t_token *t);

//parsing
t_ms	**pars(char *cmd, char **envp);
char	*check_cmd(char *str);
int		syntax_check(char *s);
char	*check_pairs(char *str);
void	free_ms(t_ms **ms);
// envp
t_envp	**make_envp(char **envp);
t_envp	*new_var(const char *s);
t_envp	*last_var(t_envp *envp);
int		add_var(t_envp **envp, t_envp *new);
void	del_var(t_envp *var);
void	clear_envp(t_envp **envp);
// ast
//  head
t_ast	*new_ast(t_op op, bool built_in, char **cmd);
void	add_cmd(t_ast **ast, t_ast *new);
t_ast	*last_head(t_ast *ast);
t_op	get_op(t_token *t);
//  functions
t_ast	**ast(char *str, t_ms **ms);
t_ast	*subshell(t_token **t, t_ms *ms);
void	handle_cmd_add(t_token **t, t_ast *new_cmd, int *first_op, t_ast **ast);
int		cmd_parser(t_token **t, t_ast **ast, t_ms *ms, int *first_op);
int		count_args(t_token *t);
bool	is_builtin(char *cmd);
void	free_ast(t_ast **ast);
void	free_ast_content(t_ast *ast);
char	*format_cmd(char *cmd, t_ms *ms, bool is_cmd);
char	*get_expanded(char *s, t_ms *ms, int *i, int *j);
char	*expand_and_quotes(char *cmd, t_ms *ms);
char	*expand_helper(char *s, t_ms *ms, int *i, int *j);
char	*handle_quotes(char *cmd, t_ms *ms);
char	*handle_expand(char *s, t_ms *ms);
char	*get_var(t_envp *envp, char *var);
//  redirections
int		handle_redir(t_token *t, t_ast *cmd, t_ms *ms, int n);
int		handle_infile(t_token *t, t_ast *ast);
int		handle_outfile(t_token *t, t_ast *ast);
int		handle_append(t_token *t, t_ast *ast);
int		handle_heredoc(t_token *t, t_ast *ast, t_ms *ms);

#endif
