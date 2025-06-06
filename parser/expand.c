/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/12 14:29:13 by tpinarli          #+#    #+#             */
/*   Updated: 2025/06/05 21:10:24 by ykadosh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static uint32_t	expand_vars(char ***env, t_token *tokens, int i, char **ptr, char **result);
static uint32_t	handle_empty_expansion(t_token *tokens, int i, char **ptr);

/*
* With the help if expand_vars() just below, rebuild_expanded_string() indexes
* through the string which tokens[i] points at, and builds 'result' step by
* step, appending to it either the environment variable, the exit status, or the
* parts of the string which are not concerned by a '$' sign. Once the whole
* string 'tokens[i].str' is processed, its memory is freed, and it is
* replaced by the newly built string that 'result' points at.
*
* ◦ returns 1 if any call to malloc() has failed
* ◦ otherwise, returns 0
*
* NOTE:
* The check for QUOTE_SINGLE at the end of the 'else if' block might seem odd,
* but it is actually very important. When a token contains a '$' followed by an
* inexisting variable - and nothing else - that variable expands to nothing at
* all, and the token becomes a NULL pointer. If there are other string tokens in
* the array following that specific token, all tokens from that point onwards
* get replaced by their neighbour - and our NULL token's new replacement might
* have been single-quoted, in which case we do not wish to try and expand it.
*/
uint32_t	rebuild_expandable_string(char ***env, t_token *tokens, int i)
{
	char		*ptr;
	char		*result;
	uint32_t	failure_flag;

	ptr = tokens[i].str;
	result = NULL;
	failure_flag = 0;
	while (*ptr)
	{
		failure_flag = expand_vars(env, tokens, i, &ptr, &result);
		if (failure_flag == 1)
			return (1);
		else if (failure_flag == 3)
			return (0);
	}
	free(tokens[i].str);
	tokens[i].str = NULL;
	tokens[i].str = result;
	return (0);
}

/*
* Return values:
* ◦ returns 1 if any call to malloc() has failed
* ◦ returns 3 if the token expands to nothing and the next string that replaces
*	tokens[i].str is either NULL or was in single quotes in the input string.
* ◦ returns 0 in all other scenarios
*/
static uint32_t	expand_vars(char ***env, t_token *tokens, int i, char **ptr, char **result)
{
	uint32_t	failure_flag;

	failure_flag = 0;
	if (**ptr == '$' && *(*ptr + 1) == '?')
		failure_flag = expand_last_exit_status(result, ptr);
	else if (**ptr == '$' && (ft_isalpha(*(*ptr + 1)) || *(*ptr + 1) == '_'))
	{
		failure_flag = expand_environment_variable(env, ptr, result);
		if (failure_flag == 2)
		{
			if (!handle_empty_expansion(tokens, i, ptr))
				return (3);
			failure_flag = 0;
		}
	}
	else
		failure_flag = append_non_expandable_str(ptr, result);
	return (failure_flag);
}

/*
* to be used when a dollar sign is followed by a non existing variable, even
* though its first character is alphabetical or a lower dash.
* This function frees the string at tokens[i].str, sets it to NULL, and if
* there is still another token after it in the 'tokens' array, all of
* tokens[i]'s data is replaced with the following token's data. The same
* operation is then done on each and every subsequent token, up until the token
* which holds a NULL 'str' member.
*
* After this loop, 'ptr' is updated to point at the new tokens[i].str. If that
* new token is single quoted, this function returns 0; otherwise, it returns 1.
*/
static uint32_t	handle_empty_expansion(t_token *tokens, int i, char **ptr)
{
	int	temp;

	temp = i;
	while (tokens[temp].str)
	{
		if (temp == i)
		{
			free(tokens[temp].str);
			tokens[temp].str = NULL;
		}
		tokens[temp].str = tokens[temp + 1].str;
		tokens[temp].quote = tokens[temp + 1].quote;
		tokens[temp].line_id = tokens[temp + 1].line_id;
		temp++;
	}
	*ptr = tokens[i].str;
	if (!*ptr || tokens[i].quote == QUOTE_SINGLE)
		return (0);
	return (1);
}
