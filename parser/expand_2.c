/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_2.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/12 14:29:13 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/01 17:47:43 by ykadosh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int32_t	expand_dollar_alpha(char **ptr);

/*
* working version: It doesn't leak, but it would leak on any of the malloc()
* failures, which are happenning within this function! Even if you would return
* NULL.
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
int32_t	expand_variables(char *str)
{
	char	*ptr;
	char	*result;
	int32_t	flag;

	ptr = str;
	result = NULL;
	flag = 0;
	while (*ptr)
	{
		if (*ptr == '$' && *(ptr + 1) == '?')
		{
			expand_dollar_question(&ptr, ???);




		}
		else if (*ptr == '$' && ft_isalpha(*(ptr + 1)))
		{
			flag = expand_dollar_alpha(&ptr);
			if (flag == -1)
				return (-1);
		}
		else
		{
			flag = expand_dollar_nonalpha();
			if (flag == -1)
				return (-1);
		}
	}
	free (str);
	return (result);
}

int32_t	expand_dollar_question(char **ptr, 
{



// returns -1 if any call to malloc() has failed
// returns 0 otherwise
static int32_t	expand_dollar_alpha(char **ptr)
{
	char	*varname;
	char	*value;
	size_t	len;

	varname = NULL;
	value = NULL;
	len = 0;
	(*ptr)++;
	while (ft_isalnum((*ptr)[len]) || (*ptr)[len] == '_')
		len++;

	varname = ft_substr(ptr, 0, len);
	if (!varname)
		return (-1);
	value = getenv(varname);
	// NOTE: do NOT free() this 'vaue' pointer! It would lead to undefined behaviour.
	// The return value of getenv() is static memory, that you should not free().
	free(varname);
	varname = NULL;
	if (value)
	{
		if (!result)
			result = ft_strdup(value);
		else
		{
			varname = result;
			result = NULL;
			result = ft_strjoin(varname, value);
			free(varname);
			varname = NULL;
		}
		if (!result)
			return (-1);
	}
	(*ptr) += len;
	return (0);
}

int32_t	expand_dollar_nonalpha(



