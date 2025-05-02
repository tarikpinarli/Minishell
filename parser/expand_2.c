/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_2.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/12 14:29:13 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/01 18:54:11 by ykadosh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int32_t	expand_last_exit_status(char **result);
static int32_t	expand_environment_variable(char **ptr, char **result);
static int32_t	append_non_expandable_str(char **ptr, char **result);
static int32_t	strjoin_and_replace(char **s1, char **s2, uint8_t is_s2_heap);

/*
* working version: It doesn't leak, but it would leak on any of the malloc()
* failures, and there are quite a few of them taking place within this function!
char	*expand_variables(char *str)
{
	const char	*ptr = str;
//	char		*result; // boş başlangıç
	char		*result = ft_calloc(1, 1); // boş başlangıç
	char		*varname;
	char		*value;
	char		*before;
	char 		*exit_str;
	size_t		len;

	while (*ptr)
	{
		if (*ptr == '$' && *(ptr + 1) == '?')
		{
			exit_str = ft_itoa(last_exit_code(0, 0));
			before = result;
			result = ft_strjoin(result, exit_str);
			free(before);
			free(exit_str);
			ptr += 2;
			continue;
		}
		else if (*ptr == '$' && ft_isalpha(*(ptr + 1)))
		{
			ptr++; // skip $ sign
			len = 0;
			while (ft_isalnum(ptr[len]) || ptr[len] == '_')
				len++;

			varname = ft_substr(ptr, 0, len);
			value = getenv(varname);
			free(varname);

			if (value)
			{
				before = result;
				result = ft_strjoin(result, value);
				free(before);
			}
			ptr += len;
		}
		else
		{
			char tmp[2] = {*ptr, '\0'};
			before = result;
			result = ft_strjoin(result, tmp);
			free(before);
			ptr++;
		}
	}
	free(str);
	return (result);
}
*/

// IMPORTANT NOTE: on each malloc() failure here, we should free up ALL of the
// .str members of tokens, by indexing from the current i index to the last
// struct in the array first, and then indexing backwards and freeing each
// .str member until i is 0!
// Otherwise, if we free from the first struct in the array until we hit a
// NULL .str member, we risk missing some members, since we have freed some.
/*
* ◦ returns -1 if any call to malloc() has failed
* ◦ otherwise, returns 0
*/
int32_t	expand_variables(t_token *tokens, int i)
{
	char	*ptr;
	char	*result;
	int32_t	failure_flag;

	ptr = tokens[i].str;
	result = NULL;
	failure_flag = 0;
	while (*ptr)
	{
		if (*ptr == '$' && *(ptr + 1) == '?')
		{
			failure_flag = expand_last_exit_status(&result);
//			if (failure_flag == -1)
//				return (-1);
			ptr += 2;
//			continue; // unnecessary since the next statements are 'else if'
			//	and 'else', so they won't execute if the current 'if' statement
			// executed. The only check the code should do before reiterating
			// in this loop is: 'if (failure_flag == -1)
		}
		else if (*ptr == '$' && ft_isalpha(*(ptr + 1)))
		{
			failure_flag = expand_environment_variable(&ptr, &result);
//			if (failure_flag == -1)
//				return (-1);
		}
		else
		{
			failure_flag = append_non_expandable_str(&ptr, &result);
//			if (failure_flag == -1)
//				return (-1);
		}

		if (failure_flag == -1)  // this if statement should be checked after each iteration of the loop.
			return (-1);
	}
	free(tokens[i].str);
	tokens[i].str = NULL;
	tokens[i].str = result;
	free (result);
	return (0);
}

/*
* ◦ returns -1 if any call to malloc() has failed
* ◦ otherwise, returns 0
*/
static int32_t	expand_last_exit_status(char **result)
{
	char	*exit_str;
//	char	*temp;

	exit_str = NULL;
//	temp = NULL;
	exit_str = ft_itoa(last_exit_code(0, 0));
	if (!exit_str)
		return (-1);


	return (strjoin_and_replace(result, &exit_str, 1));
	

/*
* inlined version of strjoin_and_replace():
	if (!*result)
		*result = ft_strdup(exit_str);
	else
	{
		temp = *result;
		*result = NULL;
		*result = ft_strjoin(temp, &exit_str);
		free(temp);
		temp = NULL;
	}
	free(exit_str);
	exit_str = NULL;
	if (!*result)
		return (-1);
	return (0);
*/
}

/*
* ◦ returns -1 if any call to malloc() has failed
* ◦ otherwise, returns 0
* NOTE: Do NOT free() the 'value' pointer! It would lead to undefined behaviour.
* The return value of getenv() (which is assigned to 'value') is static memory,
* that one should not free().
*/
static int32_t	expand_environment_variable(char **ptr, char **result)
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
		return (-1);
	value = getenv(temp);
	free(temp);
	temp = NULL;
	if (value)
	{
		if (strjoin_and_replace(result, &value, 0) == -1)
			return (-1);
		/*
		 * inlined version of strjoin_and_replace():
		if (!*result)
			*result = ft_strdup(value);
		else
		{
			temp = *result;
			*result = NULL;
			*result = ft_strjoin(temp, &value);
			free(temp);
			temp = NULL;
		}
		if (!*result)
			return (-1);
		*/
	}
	(*ptr) += len;
	return (0);
}

/*
* ◦ returns -1 if any call to malloc() has failed
* ◦ otherwise, returns 0
*/
static int32_t	append_non_expandable_str(char **ptr, char **result)
{
	size_t	len;
	char	*temp;

	// we skip the dollar sign, without modifying the original pointer
	temp = *ptr + 1;
	while (*temp)
	{
		while (*temp && *temp != '$')
			temp++;
		if (*temp == '$' && *(temp + 1) != '?' && !ft_isalpha(*(temp + 1)))
			temp++;
	}
	len = temp - *ptr;
	temp = NULL;
	temp = ft_substr(*ptr, 0, len);
	if (!temp)
		return (-1);
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
static int32_t	strjoin_and_replace(char **s1, char **s2, uint8_t is_s2_heap)
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
		return (-1);
	return (0);
}
