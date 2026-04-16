# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: alubrano <alubrano@student.42lausanne.c    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/02/05 16:33:55 by alubrano          #+#    #+#              #
#    Updated: 2026/04/08 11:12:21 by alubrano         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Colors
GREEN	= \033[0;32m
RED		= \033[0;31m
YELLOW	= \033[0;33m
RESET	= \033[0m

#Standard

NAME		= minishell
INC_DIR		= 01_inc
LIBFT_DIR	= libft
LIBFT		= $(LIBFT_DIR)/libft.a
SRCS_DIR 	= ./
OBJS_DIR 	= obj/
CC 			= gcc
RM 			= rm -f
CFLAGS 		= -Wall -Wextra -Werror -g

#Sources

MAIN = 00_src/00_main

TOKEN = 00_src/00_token/
TOK = 00_token 01_token_utils 02_free_token

PARSING = 00_src/01_parsing/
PARS = 00_pars 01_pars_utils 02_handle_pairs

ENVP = 00_src/01_parsing/00_envp/
ENV = 00_envp 01_envp_utils 02_envp_clean

AST = 00_src/01_parsing/01_ast/
AS = 00_ast 01_ast_struct_utils 02_ast_utils 03_format_cmd \
	04_format_utils 05_handle_redirections 06_free_ast

BUILTIN = 00_src/03_builtin/
BUIL = 00_execute_builtin 01_echo_builtin 02_echo_utils 03_cd_builtin 04_env_builtin \
	05_pwd_builtin 06_exit_builtin 07_export_builtin 08_export_utils 09_unset_builtin \
	env_to_char utils_builtin

EXEC = 00_src/02_exec/
EXECS = 00_ast_exec 01_exe_pipe 02_exe_sub 03_logical 04_redir 05_exec_utils 06_utils_fd \
	07_wilcard 08_wc_match_check 09_wc_add_sort

SIGNAL = 00_src/04_signal/
SIG = 00_signal 01_sigs_lock

HEREDOC = 00_src/06_heredoc/
HERE = heredoc heredoc_utils

SRC_FILES += $(MAIN)
SRC_FILES += $(addprefix $(TOKEN),$(TOK))
SRC_FILES += $(addprefix $(PARSING),$(PARS))
SRC_FILES += $(addprefix $(ENVP),$(ENV))
SRC_FILES += $(addprefix $(AST),$(AS))
SRC_FILES += $(addprefix $(BUILTIN),$(BUIL))
SRC_FILES += $(addprefix $(EXEC),$(EXECS))
SRC_FILES += $(addprefix $(SIGNAL),$(SIG))

SRC_FILES += $(addprefix $(HEREDOC),$(HERE))

SRCS = $(addprefix $(SRCS_DIR), $(addsuffix .c, $(SRC_FILES)))
OBJS = $(addprefix $(OBJS_DIR), $(addsuffix .o, $(SRC_FILES)))

###

OBJSF 		= .cache_exists
INCLUDES 	= -I$(INC_DIR) -I$(LIBFT_DIR)

all: ${NAME}

bonus: $(LIBFT) $(OBJS)
	@$(CC) $(CFLAGS) -D BONUS=1 $(OBJS) $(LIBFT) -lreadline -o $(NAME)
	@echo -e "$(GREEN)✓ Build OK: libft$(RESET)"
	@echo -e "$(GREEN)✓ Build OK: bonus $(NAME)$(RESET)"

$(LIBFT):
	@make -C $(LIBFT_DIR)

$(NAME): $(LIBFT) $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) $(LIBFT) -lreadline -o $(NAME)
	@echo -e "$(GREEN)✓ Build OK: libft$(RESET)"
	@echo -e "$(GREEN)✓ Build OK: $(NAME)$(RESET)"

$(OBJS_DIR)%.o : $(SRCS_DIR)%.c | $(OBJSF)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJSF):
	@mkdir -p $(OBJS_DIR)
	@mkdir -p $(OBJS_DIR)$(TOKEN)
	@mkdir -p $(OBJS_DIR)$(PARSING)
	@mkdir -p $(OBJS_DIR)$(ENVP)
	@mkdir -p $(OBJS_DIR)$(AST)
	@mkdir -p $(OBJS_DIR)$(EXEC)
	@mkdir -p $(OBJS_DIR)$(BUILTIN)
	@mkdir -p $(OBJS_DIR)$(SIGNAL)
	@mkdir -p $(OBJS_DIR)$(OPERATOR)
	@mkdir -p $(OBJS_DIR)$(HEREDOC)

clean:
	@$(RM) -rf $(OBJS_DIR)
	@$(RM) -f $(OBJSF)
	@echo -e "$(RED)✗ Objects removed$(RESET)"

fclean: clean
	@rm -f $(NAME)
	@echo -e "$(RED)✗ $(NAME) removed$(RESET)"

re: fclean all

valgrind: all
	@echo -e "$(YELLOW)🔍 Running valgrind...$(RESET)"
	@valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes \
		--suppressions=readline.supp --log-fd=2 ./$(NAME) "$(ARGS)"

.PHONY : all clean fclean re valgrind
