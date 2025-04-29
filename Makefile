# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/04/11 10:27:49 by tpinarli          #+#    #+#              #
#    Updated: 2025/04/28 16:35:50 by tpinarli         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = minishell

CC = cc
CFLAGS = -Wall -Wextra -Werror

VALGRIND = -g
FSANITIZE = -fsanitize=leak -fsanitize=address -fsanitize=undefined

INCLUDES = -I. -Ilibft
HEADER = $(NAME).h

GREEN = \033[0;32m
YELLOW = \033[1;33m
RESET = \033[0m

SRC_DIR = .
OBJ_DIR = obj
LIBFT_DIR = libft
BIN_DIR = bin

SRC = main.c \
	parser/tokenizer.c \
	parser/tokenizer_utils.c \
	parser/parser_utils.c \
	parser/parser.c \
	parser/expand.c \
	parser/expand_tokens.c \
	parser/merge_tokens.c \
	executor/exec_cmd.c \
	executor/redirection.c \
	builtins/builtin.c \
	builtins/builtin_echo.c \
	builtins/builtin_cd.c \
	builtins/builtin_env.c \
	builtins/builtin_exit.c \
	builtins/builtin_export.c \
	builtins/builtin_pwd.c \
	builtins/builtin_unset.c \
	utils/signals.c \
	utils/env_utils.c \
	utils/memory.c \
	utils/status.c \

OBJ = $(addprefix $(OBJ_DIR)/, $(SRC:.c=.o))

LIBFT = $(LIBFT_DIR)/libft.a

all: $(NAME)

$(NAME): libft
	@make CFLAGS="$(CFLAGS)" --no-print-directory $(BIN_DIR)/$@

$(BIN_DIR)/$(NAME): $(LIBFT) $(OBJ) | $(BIN_DIR)
	@echo "$(YELLOW)[Linking]$(RESET) $(NAME)"
	@$(CC) $(CFLAGS) $(INCLUDES) $(OBJ) $(LIBFT) -lreadline -lncurses -o $@
	@echo "$(GREEN)✅ Build complete!$(RESET)"

$(OBJ_DIR)/%.o: %.c $(HEADER)
	@mkdir -p $(dir $@)
	@echo "$(YELLOW)[Compiling]$(RESET) $<"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BIN_DIR):
	@mkdir -p $@

libft:
	@echo "$(YELLOW)[Libft]$(RESET) Building libft, if not already built..."
	@make --no-print-directory -C $(LIBFT_DIR) CFLAGS="$(CFLAGS)"

clean:
	@echo "$(YELLOW)[Cleaning]$(RESET) object files..."
	@make clean -C $(LIBFT_DIR)
	@rm -rf $(OBJ_DIR)

fclean: clean
	@echo "$(YELLOW)[Cleaning]$(RESET) executable..."
	rm -f $(LIBFT)
	rm -rf $(BIN_DIR)

re: fclean all

valgrind: CFLAGS += $(VALGRIND)
valgrind: re

fsanitize: CFLAGS += $(FSANITIZE)
fsanitize: re

.PHONY: all clean fclean re libft valgrind fsanitize
