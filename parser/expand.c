/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/12 14:29:13 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/06 17:51:29 by ykadosh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static uint32_t	expand_last_exit_status(char **result);
static uint32_t	expand_environment_variable(char **ptr, char **result);
static uint32_t	append_non_expandable_str(char **ptr, char **result);
static uint32_t	strjoin_and_replace(char **s1, char **s2, uint8_t is_s2_heap);

/*
* ◦ returns 1 if any call to malloc() has failed
* ◦ otherwise, returns 0
*/
uint32_t	expand_variables(t_token *tokens, int i)
{
	char		*ptr;
	char		*result;
	uint32_t	failure_flag;
	int			temp;

	ptr = tokens[i].str;
	result = NULL;
	failure_flag = 0;
	while (*ptr)
	{
		if (*ptr == '$' && *(ptr + 1) == '?')
		{
			failure_flag = expand_last_exit_status(&result);
			ptr += 2;
		}
		else if (*ptr == '$' && ft_isalpha(*(ptr + 1))) // WARN: should we add here: && *(ptr + 1) != '_') ??? UPDATE: yes we should, Yonatan will do it
		{
			failure_flag = expand_environment_variable(&ptr, &result);
			if (failure_flag == 2)
			{
				// FIXME: note to Yonatan: We need to make this block of code
				// update the token.quote element as well, so that the token.str
				// matches with the right token.quote !!!
				// TODO: Also, refactor this block into a more user friendly
				// helper function!
				temp = i;
				while (tokens[temp].str)
				{
					if (temp == i)
					{
						free(tokens[temp].str);
						tokens[temp].str = NULL;
					}
					tokens[temp].str = tokens[temp + 1].str;
					temp++;
				}
				ptr = tokens[i].str;
				if (!ptr)
					return (0);
				failure_flag = 0;
			}
		}
		else
			failure_flag = append_non_expandable_str(&ptr, &result);
		if (failure_flag)  // this if statement should be checked after each iteration of the loop.
			return (1);
	}
	free(tokens[i].str);
	tokens[i].str = NULL;
	tokens[i].str = result;
	return (0);
}

/*
* ◦ returns 1 if any call to malloc() has failed
* ◦ otherwise, returns 0
*/
static uint32_t	expand_last_exit_status(char **result)
{
	char	*exit_str;

	exit_str = NULL;
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
*	replaced by the next tokens following it. This is done when after returning
*	to expand_variables().
* ◦ otherwise, this function returns 0
* NOTE: Do NOT free() the 'value' pointer! It would lead to undefined behaviour.
* The return value of getenv() (which is assigned to 'value') is static memory,
* that one should not free().
*/
static uint32_t	expand_environment_variable(char **ptr, char **result)
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
	value = getenv(temp);
	free(temp);
	temp = NULL;
	if (value)
	{
		if (strjoin_and_replace(result, &value, 0) == 1)
			return (1);
	}
	else
	{
		if (!(*ptr)[len] && !*result)
			return (2);
	}
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
			break;
	}
	len = temp - *ptr;
	temp = NULL;
	temp = ft_substr(*ptr, 0, len);
	if (!temp)
		return (1);
	*ptr += len;
	return (strjoin_and_replace(result, &temp, 1));
}

/*
* Performs a regular strjoin of s1 and s2, but with a twist:
* - the joined result is assigned to s1 (since it is a double pointer, there is
*   no need to assign the call of this function in the caller.
* - s1 is dynamically allocated: therefore, assigning a different string to it
*   requires freeing of its memory first - which this function takes care of.
* - the last parameter 'is_s2_heap' lets this function know if the 2nd argument,
*   s2, is allocated on the heap as well and needs freeing before the function's
*   end. This is particularly useful here since s2 is the return value of
*   getenv(), this pointer should NOT be freed.
*
*   Return values: 0 upon success, 1 upon malloc() failure
*/
static uint32_t	strjoin_and_replace(char **s1, char **s2, uint8_t is_s2_heap)
{
	char	*temp;

	temp = NULL;
	if (!*s1)
		*s1 = ft_strdup(*s2);
	else
	{
		temp = *s1;
		*s1 = NULL;
		*s1 = ft_strjoin(temp, *s2);
		free(temp);
		temp = NULL;
	}
	if (is_s2_heap)
	{
		free(*s2);
		*s2 = NULL;
	}
	if (!*s1)
		return (1);
	return (0);
}
