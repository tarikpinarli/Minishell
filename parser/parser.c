/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 15:16:33 by tpinarli          #+#    #+#             */
/*   Updated: 2025/06/08 13:33:56 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
/*
 * Handles pipe tokens by validating their position and
 * creating a new t_command node in the pipeline.
 * Ensures that a pipe is not the first or last token,
 * and is not followed by another pipe.
 *
 * Returns:
 * - 1 if the pipe is valid and a new command is added
 * - -1 if a syntax error is detected and cleanup is performed
 */
static int	handle_pipe( t_token *tokens, char *input,
	t_parse_state *st, char ***env)
{
	int	i;

	i = st->i;
	if (!st->current || !tokens[i + 1].str
		|| !ft_strcmp(tokens[i + 1].str, "|"))
	{
		if (!tokens[i + 1].str)
			ft_putendl_fd("syntax error: parsing failed.", 2);
		else
			ft_putendl_fd("syntax error near unexpected token `|'", 2);
		(void)last_exit_code(1, 2);
		free_all(&input, &tokens, &st->head);
		return (-1);
	}
	st->new_cmd = (t_command *)ft_calloc(1, sizeof(t_command));
	if (!st->new_cmd)
		handle_malloc_err(input, tokens, st->head, env);
	st->current->next = st->new_cmd;
	st->current = st->new_cmd;
	st->new_cmd = NULL;
	st->i++;
	return (1);
}

/*
 * Identifies the type of redirection based on the given token string.
 * Only unquoted tokens are considered valid redirection operators.
 *
 * Returns:
 * - REDIR_IN for "<"
 * - REDIR_OUT for ">"
 * - REDIR_HEREDOC for "<<"
 * - REDIR_APPEND for ">>"
 * - REDIR_NONE if the token is not a redirection or is quoted
 */
static t_redir_type	get_redirection_id(const char *str, t_quote_type quote)
{
	if (quote != QUOTE_NONE)
		return (REDIR_NONE);
	if (*str == '<')
	{
		str++;
		if (!*str)
			return (REDIR_IN);
		if (*str == '<' && !*(str + 1))
			return (REDIR_HEREDOC);
	}
	else if (*str == '>')
	{
		str++;
		if (!*str)
			return (REDIR_OUT);
		if (*str == '>' && !*(str + 1))
			return (REDIR_APPEND);
	}
	return (REDIR_NONE);
}

/*
 * Validates the token following a redirection operator to ensure it is
 * a valid target (i.e., not empty, not another redirection, and not a pipe).
 * If invalid, prints an appropriate syntax error and frees all resources.
 *
 * Returns:
 * - 0 if the target is valid
 * - -1 if a syntax error is detected and cleanup is performed
 */
static int	validate_redir_target(t_token *tokens,
	char *input, t_parse_state *st)
{
	if (!tokens[st->i].str)
	{
		ft_putendl_fd("syntax error near unexpected token `newline'", 2);
		(void)last_exit_code(1, 2);
		free_all(&input, &tokens, &st->head);
		return (-1);
	}
	if (get_redirection_id(tokens[st->i].str, tokens[st->i].quote))
	{
		ft_putstr_fd("syntax error near unexpected token `", 2);
		write(2, tokens[st->i].str, ft_strlen(tokens[st->i].str));
		write(2, "'\n", 2);
		(void)last_exit_code(1, 2);
		free_all(&input, &tokens, &st->head);
		return (-1);
	}
	if (!ft_strcmp(tokens[st->i].str, "|"))
	{
		ft_putendl_fd("syntax error near unexpected token `|'", 2);
		(void)last_exit_code(1, 2);
		free_all(&input, &tokens, &st->head);
		return (-1);
	}
	return (0);
}

/*
 * Checks if the current token represents a redirection operator.
 * If so, validates the redirection target and appends the redirection
 * to the current command's redirection list.
 *
 * Returns:
 * - 1 if a redirection was handled successfully
 * - 0 if the token is not a redirection operator
 * - -1 if a syntax or memory error occurred
 */
static int	handle_redirection(t_token *tok, char *input,
	t_parse_state *st, char ***env)
{
	t_redir_type	redir_id;
	int				status;

	redir_id = get_redirection_id(tok[st->i].str, tok[st->i].quote);
	if (!redir_id)
		return (0);
	st->i++;
	status = validate_redir_target(tok, input, st);
	if (status == -1)
		return (-1);
	st->new_redir = create_redir(redir_id, tok[st->i].str, tok[st->i].quote);
	if (!st->new_redir)
		handle_malloc_err(input, tok, st->head, env);
	append_redir(redir_id, st->new_redir, st->current);
	return (1);
}

/*
 * Parses the tokenized input and constructs a linked list of t_command
 * structs representing a pipeline of commands with their arguments
 * and redirections.
 *
 * Handles:
 * - Pipe separators (|) between commands
 * - Input/output redirections (<, >, <<, >>)
 * - Syntax error detection (e.g., unexpected pipe or redirection)
 *
 * Returns the head of the command list on success,
 * or NULL if a syntax or memory error occurs.
 */
t_command	*parse_tokens(t_token *tokens, char *input, char ***env)
{
	t_parse_state	st;
	int				ret;

	st.head = NULL;
	st.current = NULL;
	st.new_cmd = NULL;
	st.new_redir = NULL;
	st.i = 0;
	while (tokens[st.i].str)
	{
		if (!ft_strcmp(tokens[st.i].str, "|"))
		{
			if (handle_pipe(tokens, input, &st, env) == -1)
				return (NULL);
			continue ;
		}
		init_command_if_needed(input, tokens, &st, env);
		ret = handle_redirection(tokens, input, &st, env);
		if (ret == -1)
			return (NULL);
		else if (ret == 0)
			append_argument(tokens, input, &st, env);
		st.i++;
	}
	return (st.head);
}
