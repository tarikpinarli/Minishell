/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/12 14:29:13 by tpinarli          #+#    #+#             */
/*   Updated: 2025/06/04 12:57:59 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static uint32_t	expand_vars(char ***env, t_token *tokens, int i, char **ptr, char **result);
static uint32_t	expand_last_exit_status(char **result, char **ptr);
static uint32_t	expand_environment_variable(char ***env, char **ptr, char **result);
static uint32_t	append_non_expandable_str(char **ptr, char **result);

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
* ◦ returns 1 if any call to malloc() has failed
* ◦ otherwise, returns 0
*/
static uint32_t	expand_last_exit_status(char **result, char **ptr)
{
	char	*exit_str;

	exit_str = NULL;
	*ptr += 2;
	exit_str = ft_itoa(last_exit_code(0, 0));
	if (!exit_str)
		return (1);
	return (strjoin_and_replace(result, &exit_str, 1));
}

/*
* ◦ returns 1 if any call to malloc() has failed
* ◦ returns 2 if a non existing variable was about to be expendad, and that
*	variable was the only data that a token was holding; in this case, Minishell
*	expands that to nothing, and so the token has to become empty, and should be
*	replaced by the next tokens following it.
* ◦ otherwise, this function returns 0
*
* NOTE: Do NOT free() the 'value' pointer! It would lead to undefined behaviour.
* The return value of getenv() (which is assigned to 'value') is static memory,
* that one should not free().
*/
static uint32_t	expand_environment_variable(char ***env, char **ptr, char **result)
{
	char	*temp;
	char	*value;
	size_t	len;

	temp = NULL;
	value = NULL;
	len = 0;
	(*ptr)++;
	while (ft_isalnum((*ptr)[len]) || (*ptr)[len] == '_')
		len++;
	temp = ft_substr(*ptr, 0, len);
	if (!temp)
		return (1);
	value = get_env_value(*env, temp);
	free(temp);
	if (value)
	{
		if (strjoin_and_replace(result, &value, 0) == 1)
			return (1);
	}
	else
		if (!(*ptr)[len] && !*result)
			return (2);
	(*ptr) += len;
	return (0);
}

/*
* ◦ returns 1 if any call to malloc() has failed
* ◦ otherwise, returns 0
*/
static uint32_t	append_non_expandable_str(char **ptr, char **result)
{
	size_t	len;
	char	*temp;

	temp = *ptr + 1;
	while (*temp)
	{
		while (*temp && *temp != '$')
			temp++;
		if (*temp == '$' && *(temp + 1) != '?' && !ft_isalpha(*(temp + 1)))
			temp++;
		else
			break ;
	}
	len = temp - *ptr;
	temp = NULL;
	temp = ft_substr(*ptr, 0, len);
	if (!temp)
		return (1);
	*ptr += len;
	return (strjoin_and_replace(result, &temp, 1));
}
