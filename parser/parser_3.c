/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_3.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/08 13:16:44 by tpinarli          #+#    #+#             */
/*   Updated: 2025/06/08 13:33:13 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
 * Appends the current token to the argv list of the current command.
 * Handles memory errors by freeing all resources and exiting.
 */
void	append_argument(t_token *tokens, char *input,
	t_parse_state *st, char ***env)
{
	st->current->argv = argv_add(st->current->argv, tokens[st->i].str);
	if (!st->current->argv)
		handle_malloc_err(input, tokens, st->head, env);
}

/*
 * Handles memory allocation failures by freeing all allocated resources,
 * clearing readline history, printing an error message, and exiting
 * with the appropriate exit code.
 */
void	handle_malloc_err(char *input, t_token *tokens,
	t_command *head, char ***env)
{
	free_all(&input, &tokens, &head);
	free_two_dimensional_array(env);
	rl_clear_history();
	write(2, ALLOCATION_FAILURE, sizeof(ALLOCATION_FAILURE) - 1);
	exit (last_exit_code(1, 1));
}

/*
 * Initializes the first command node if it hasn't been created yet.
 * Allocates memory for the command and updates the parser state.
 * On failure, handles the error and exits.
 */
void	init_command_if_needed(char *input, t_token *tokens,
	t_parse_state *st, char ***env)
{
	if (!st->current)
	{
		st->current = (t_command *)ft_calloc(1, sizeof(t_command));
		if (!st->current)
			handle_malloc_err(input, tokens, st->head, env);
		st->head = st->current;
	}
}
