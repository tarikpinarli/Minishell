/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykadosh <ykadosh@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 14:55:45 by ykadosh           #+#    #+#             */
/*   Updated: 2025/06/05 21:23:02 by ykadosh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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
uint32_t	strjoin_and_replace(char **s1, char **s2, uint8_t is_s2_heap)
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

/*
* ◦ returns 1 if any call to malloc() has failed
* ◦ otherwise, returns 0
*/
uint32_t	expand_last_exit_status(char **result, char **ptr)
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
* ◦ otherwise, returns 0
*/
uint32_t	append_non_expandable_str(char **ptr, char **result)
{
	size_t	len;
	char	*temp;

	temp = *ptr + 1;
	while (*temp)
	{
		while (*temp && *temp != '$')
			temp++;
		if (*temp == '$' && *(temp + 1) != '?' && *(temp + 1) != '_'
			&& !ft_isalpha(*(temp + 1)))
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

/*
* ◦ returns 1 if any call to malloc() has failed
* ◦ returns 2 if a non existing variable was about to be expanded, and that
*	variable was the only data that a token was holding; in this case, Minishell
*	expands that to nothing, and so the token has to become empty, and should be
*	replaced by the next tokens following it.
* ◦ otherwise, this function returns 0
*
* NOTE: The 'value' pointer is not to be freed: It is part of the environment
* copy which Minishell holds and updates, and it should stay accessible for the
* program.
*/
uint32_t	expand_environment_variable(char ***env, char **ptr, char **result)
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
