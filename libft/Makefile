#* ************************************************************************** *#
#*                                                                            *#
#*                                                        :::      ::::::::   *#
#*   Makefile                                           :+:      :+:    :+:   *#
#*                                                    +:+ +:+         +:+     *#
#*   By: dcresce <dcresce@student.42lausanne.ch>    +#+  +:+       +#+        *#
#*                                                +#+#+#+#+#+   +#+           *#
#*   Created: 18/11/2025 14:32:59 by dcresce           #+#    #+#             *#
#*   Updated: 19/11/2025 16:02:19 by dcresce          ###   ########.ch       *#
#*                                                                            *#
#* ************************************************************************** *#

#Standard

NAME = libft.a
INCLUDES = ./
SRCS_DIR = ./
OBJS_DIR = obj/
LIBC = ar rcs
CC = gcc
RM = rm -f
CFLAGS = -Wall -Wextra -Werror -I

#Sources

IS_PATH = 00_is_x/
IS = ft_isalpha ft_isdigit ft_isalnum ft_isascii ft_isprint ft_isbetween

STR_PATH = 01_str/
STR = ft_strlen ft_strlcpy ft_strlcat ft_strchr ft_strrchr \
	ft_strncmp ft_strnstr ft_strdup ft_substr ft_strjoin \
	ft_strtrim ft_strmapi ft_striteri ft_strsjoin

MEM_PATH = 02_mem/
MEM = ft_memset ft_bzero ft_memcpy ft_memmove ft_memchr \
	ft_memcmp ft_calloc

LL_PATH = 03_LinkedList/
LL = ft_lstnew ft_lstadd_front ft_lstsize ft_lstlast ft_lstadd_back \
	ft_lstdelone ft_lstclear ft_lstiter ft_lstmap

PUT_PATH = 04_put/
PUT = ft_putchar_fd ft_putstr_fd ft_putendl_fd ft_putnbr_fd ft_putnbr_hex_fd \
	ft_putptr_fd ft_putunbr_fd ft_printf

CONV_PATH = 05_conv/
CONV = ft_toupper ft_tolower ft_atoi ft_split ft_itoa ft_atol free_split

MATHS_PATH = 06_maths/
MATHS = ft_sqrt

MISC_PATH = 07_misc/
MISC = get_next_line

SRC_FILES += $(addprefix $(IS_PATH),$(IS))
SRC_FILES += $(addprefix $(STR_PATH),$(STR))
SRC_FILES += $(addprefix $(MEM_PATH),$(MEM))
SRC_FILES += $(addprefix $(LL_PATH),$(LL))
SRC_FILES += $(addprefix $(PUT_PATH),$(PUT))
SRC_FILES += $(addprefix $(CONV_PATH),$(CONV))
SRC_FILES += $(addprefix $(MATHS_PATH),$(MATHS))
SRC_FILES += $(addprefix $(MISC_PATH),$(MISC))

SRCS = $(addprefix $(SRCS_DIR), $(addsuffix .c, $(SRC_FILES)))
OBJS = $(addprefix $(OBJS_DIR), $(addsuffix .o, $(SRC_FILES)))

###

OBJSF = .cache_exists

all: ${NAME}

${NAME}: ${OBJS}
	@${LIBC} ${NAME} ${OBJS}
	@ranlib $(NAME)
	@echo "libft created"

$(OBJS_DIR)%.o : $(SRCS_DIR)%.c | $(OBJSF)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJSF):
	@mkdir -p $(OBJS_DIR)
	@mkdir -p $(OBJS_DIR)$(IS_PATH)
	@mkdir -p $(OBJS_DIR)$(STR_PATH)
	@mkdir -p $(OBJS_DIR)$(MEM_PATH)
	@mkdir -p $(OBJS_DIR)$(LL_PATH)
	@mkdir -p $(OBJS_DIR)$(PUT_PATH)
	@mkdir -p $(OBJS_DIR)$(CONV_PATH)
	@mkdir -p $(OBJS_DIR)$(MATHS_PATH)
	@mkdir -p $(OBJS_DIR)$(MISC_PATH)

clean:
	@$(RM) -rf $(OBJS_DIR)
	@$(RM) -f $(OBJSF)

fclean: clean
	@${RM} -f ${NAME}

re: fclean all

.PHONY : all clean fclean re
