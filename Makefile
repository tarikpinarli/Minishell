# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/04/11 10:27:49 by tpinarli          #+#    #+#              #
#    Updated: 2025/04/11 11:46:04 by tpinarli         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = minishell

CC = cc
CFLAGS = -Wall -Wextra -Werror -g
INCLUDES = -I. -Ilibft

# Directories
SRC_DIR = .
OBJ_DIR = obj
LIBFT_DIR = libft

# Source files
SRC = main.c \
      parser/tokenizer.c \
      parser/parser_utils.c \
      executor/exec_cmd.c \
      executor/redirection.c \
      builtins/echo.c \
      builtins/cd.c \
      builtins/env.c \
      utils/signals.c \
      utils/env_utils.c \
      utils/memory.c

# Object files with path redirected to obj/
OBJ = $(addprefix $(OBJ_DIR)/, $(SRC:.c=.o))

# Libft
LIBFT = $(LIBFT_DIR)/libft.a

# Rules
all: $(NAME)

$(NAME): $(LIBFT) $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJ) $(LIBFT) -lreadline -lncurses -o $(NAME)

$(LIBFT):
	make -C $(LIBFT_DIR)

# Compile source files into obj/
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	make clean -C $(LIBFT_DIR)
	rm -rf $(OBJ_DIR)

fclean: clean
	make fclean -C $(LIBFT_DIR)
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re