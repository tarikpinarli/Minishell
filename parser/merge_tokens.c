/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   merge_tokens.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykadosh <ykadosh@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 12:07:44 by ykadosh           #+#    #+#             */
/*   Updated: 2025/05/08 18:06:42 by ykadosh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// NOTE: *ptr is called as such because it is not the same as *tokens;
// rather, it is a pointer to the token struct containing the first string
// we are about to join with other ones.
static char	*join_multiple_strs(t_token *ptr, size_t n_strs_to_join, size_t len)
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

// this function returns 0 upon failure.
// 1 upon success.
t_token	*merge_tokens(t_token *tokens)
{
	size_t	i;
	size_t	j;
	size_t	k;
	size_t	len;
	
	if (!tokens)
		return (NULL);

	len = 0;
	i = 0;
	k = 0;
	printf("in merge_tokens(), tokens are:\n\n\n");
	while (tokens[i].str)
	{
		printf("\tvalue of k:	<%zu>\n\ttokens[k].str is:	<%s>\n\ttokens[k].quote is:	<%d>\n\ttokens[k].line_id is:	<%d>\n\n\n",
			k, tokens[k].str, tokens[k].quote, tokens[k].line_id);
		j = i;
		if (tokens[i + 1].str && tokens[i].line_id == tokens[i + 1].line_id)
		{
			while (tokens[j + 1].str && tokens[j].line_id == tokens[j + 1].line_id)
			{
				len += ft_strlen(tokens[j].str);
				j++;
			}
			len += ft_strlen(tokens[j].str);
			tokens[k].str = join_multiple_strs(&tokens[i], j - i, len);
			if (!tokens[k].str)
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
				return (NULL);
			}
			tokens[k].quote = tokens[i].quote; // new addition here
			tokens[k].line_id = tokens[i].line_id; // new addition here
		}
		else if (k != i)
		{
			tokens[k].str = ft_strdup(tokens[i].str);
			if (!tokens[k].str) // this is the exact same freeing block as you have a little bit earlier.
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
				return (NULL);
			}
			free(tokens[i].str);
			tokens[i].str = NULL;
			tokens[k].quote = tokens[i].quote; // new addition here
			tokens[k].line_id = tokens[i].line_id; // new addition here
		}
		i = j + 1;
		k++;
		printf("value of i at the end of the loop:	<%zu>\n", i);
	}
	while (tokens[k].str)
	{
		free(tokens[k].str);
		tokens[k].str = NULL;
		k++;
	}

	return (tokens);
}
