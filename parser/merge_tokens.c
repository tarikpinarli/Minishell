/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   merge_tokens.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykadosh <ykadosh@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 12:07:44 by ykadosh           #+#    #+#             */
/*   Updated: 2025/05/08 20:46:04 by ykadosh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	merge_string(t_token *tokens, size_t i, size_t *j, size_t k);
static int	replace_string(t_token *tokens, size_t i, size_t k);
static char	*strjoin_multiple(t_token *ptr, size_t n_strs_to_join, size_t len);
static void	free_tokens_input_and_exit(t_token *tokens, char *input, size_t i);

void	merge_tokens(t_token *tokens, char *input)
{
	size_t	i;
	size_t	j;
	size_t	k;

	i = 0;
	k = 0;
	while (tokens[i].str)
	{
		j = i;
		if (tokens[i].line_id != -1 && tokens[i + 1].str
			&& tokens[i].line_id == tokens[i + 1].line_id)
		{
			if (merge_string(tokens, i, &j, k) == -1)
				free_tokens_input_and_exit(tokens, input, i);
		}
		else if (k != i)
		{
			if (replace_string(tokens, i, k) == -1)
				free_tokens_input_and_exit(tokens, input, i);
		}
		i = j + 1;
		k++;
	}
	free_deprecated_strings(tokens, k);
}

static int	merge_string(t_token *tokens, size_t i, size_t *j, size_t k)
{
	size_t	len;

	len = 0;
	while (tokens[*j + 1].str && tokens[*j].line_id == tokens[*j + 1].line_id)
	{
		len += ft_strlen(tokens[*j].str);
		(*j)++;
	}
	len += ft_strlen(tokens[*j].str);
	tokens[k].str = strjoin_multiple(&tokens[i], *j - i, len);
	if (!tokens[k].str)
		return (-1);
	tokens[k].quote = tokens[i].quote;
	tokens[k].line_id = tokens[i].line_id;
	return (0);
}

static int	replace_string(t_token *tokens, size_t i, size_t k)
{
	tokens[k].str = ft_strdup(tokens[i].str);
	if (!tokens[k].str)
		return (-1);
	free(tokens[i].str);
	tokens[i].str = NULL;
	tokens[k].quote = tokens[i].quote;
	tokens[k].line_id = tokens[i].line_id;
	return (0);
}

/*
* NOTE: *ptr is called as such because it is not the same as *tokens;
* rather, it is a pointer to the token struct containing the first string we
* are about to join with other ones.
*/
static char	*strjoin_multiple(t_token *ptr, size_t n_strs_to_join, size_t len)
{
	char	*result;
	size_t	i;
	size_t	temp_len;

	result = NULL;
	result = (char *)ft_calloc(1, len + 1);
	if (!result)
		return (NULL);
	len = ft_strlen(ptr->str);
	(void)ft_memmove(result, ptr->str, len);
	free(ptr->str);
	ptr->str = NULL;
	temp_len = 0;
	i = 1;
	while (i <= n_strs_to_join)
	{
		temp_len = ft_strlen(ptr[i].str);
		(void)ft_memmove(&result[len], ptr[i].str, temp_len);
		len += temp_len;
		free(ptr[i].str);
		ptr[i].str = NULL;
		i++;
	}
	return (result);
}

/*
* This function is customly made for the context of merge_tokens(), where, in
* some scenarios, freeing of the strings allocated within the array of structs
* 'tokens' should be done in reverse, from the very last string backwards, as
* one or more of the strings in the middle of the array might be NULL while
* merge_tokens() is executing - but all strings at tokens' index i onwards are
* assured to be valid (until the last NULL, which marks the end of the array).
*/
static void	free_tokens_input_and_exit(t_token *tokens, char *input, size_t i)
{
	while (tokens[i].str)
		i++;
	while (i > 0)
	{
		i--;
		free(tokens[i].str);
		tokens[i].str = NULL;
	}
	free(tokens);
	free(input);
	exit (last_exit_code(1, 1));
}
