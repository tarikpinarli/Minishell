/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_expansion.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/12 14:29:13 by tpinarli          #+#    #+#             */
/*   Updated: 2025/06/05 18:21:41 by ykadosh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static uint32_t	index_and_expand_line(char ***env, char **ptr, char **result);
static uint32_t	expand_environment_variable_heredoc(char ***env, char **ptr,
				char **result);

uint32_t	rebuild_expandable_heredoc_line(char **line, char ***env)
{
	char		*ptr;
	char		*result;
	uint32_t	failure_flag;

	ptr = *line;
	result = NULL;
	failure_flag = 0;
	while (*ptr)
	{
		failure_flag = index_and_expand_line(env, &ptr, &result);
		if (failure_flag == 1)
			return (1);
	}
	free(*line);
	*line = result;
	return (0);
}

/*
* Return values:
* ◦ returns 1 if any call to malloc() has failed
* ◦ returns 0, otherwise
*/
static uint32_t	index_and_expand_line(char ***env, char **ptr, char **result)
{
	uint32_t	failure_flag;

	failure_flag = 0;
	if (**ptr == '$' && *(*ptr + 1) == '?')
		failure_flag = expand_last_exit_status(result, ptr);
	else if (**ptr == '$' && (ft_isalpha(*(*ptr + 1)) || *(*ptr + 1) == '_'))
		failure_flag = expand_environment_variable_heredoc(env, ptr, result);
	else
		failure_flag = append_non_expandable_str(ptr, result);
	return (failure_flag);
}

/*
* ◦ returns 1 if any call to malloc() has failed
* ◦ otherwise, returns 0.
*/
static uint32_t	expand_environment_variable_heredoc(char ***env, char **ptr,
				char **result)
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
		if (strjoin_and_replace(result, &value, 0) == 1)
			return (1);
	(*ptr) += len;
	return (0);
}
