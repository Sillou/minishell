/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   structs.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alubrano <alubrano@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/19 22:15:53 by dcresce           #+#    #+#             */
/*   Updated: 2026/04/03 11:12:35 by alubrano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRUCTS_H
# define STRUCTS_H

# include "minishell.h"

typedef enum e_op
{
	OP_NONE,
	OP_AND,
	OP_OR,
	OP_PIPE,
	OP_SUB
}	t_op;

typedef enum e_redir_type
{
	R_IN,
	R_OUT,
	R_APPEND,
	R_HEREDOC
}	t_redir_type;

typedef struct s_redir
{
	t_redir_type	type;
	char			*file;
	int				fd;
	struct s_redir	*next;
}	t_redir;

typedef struct s_ast
{
	t_op			op;
	struct s_ast	*left;
	struct s_ast	*right;
	char			**cmd;
	bool			built_in;
	t_redir			*redir;
}	t_ast;

typedef struct s_envp
{
	char			*var;
	char			*content;
	struct s_envp	*next;
}	t_envp;

typedef struct s_ms
{
	t_ast	**cmds;
	t_envp	**envp;
	char	**path;
	char	*c_dir;
	int		exit_code;

}	t_ms;

typedef struct s_pipe
{
	int		fds[2];
	int		left_pid;
	int		righ_pid;
}	t_pipe;

#endif
