# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/04/11 10:27:49 by tpinarli          #+#    #+#              #
#    Updated: 2025/04/20 15:59:29 by tpinarli         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = minishell

CC = cc
CFLAGS = -Wall -Wextra -Werror -g
INCLUDES = -I. -Ilibft

GREEN = \033[0;32m
YELLOW = \033[1;33m
RESET = \033[0m

SRC_DIR = .
OBJ_DIR = obj
LIBFT_DIR = libft

SRC = main.c \
      parser/tokenizer.c \
	  parser/tokenizer_utils.c \
      parser/parser_utils.c \
	  parser/parser.c \
	  parser/expand.c \
      executor/exec_cmd.c \
      executor/redirection.c \
      builtins/echo.c \
      builtins/cd.c \
      builtins/env.c \
      utils/signals.c \
      utils/env_utils.c \
      utils/memory.c \
	  utils/status.c \

OBJ = $(addprefix $(OBJ_DIR)/, $(SRC:.c=.o))

LIBFT = $(LIBFT_DIR)/libft.a

all: $(NAME)

$(NAME): $(LIBFT) $(OBJ)
	@echo "$(YELLOW)[Linking]$(RESET) $(NAME)"
	@$(CC) $(CFLAGS) $(INCLUDES) $(OBJ) $(LIBFT) -lreadline -lncurses -o $(NAME)
	@echo "$(GREEN)✅ Build complete!$(RESET)"

$(LIBFT):
	@echo "$(YELLOW)[Libft]$(RESET) Building libft..."
	@make -C $(LIBFT_DIR)

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo "$(YELLOW)[Compiling]$(RESET) $<"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@echo "$(YELLOW)[Cleaning]$(RESET) object files..."
	@make clean -C $(LIBFT_DIR)
	@rm -rf $(OBJ_DIR)

fclean: clean
	@echo "$(YELLOW)[Cleaning]$(RESET) executable..."
	@make fclean -C $(LIBFT_DIR)
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
