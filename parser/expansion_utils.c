/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykadosh <ykadosh@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 14:55:45 by ykadosh           #+#    #+#             */
/*   Updated: 2025/05/08 17:07:06 by ykadosh          ###   ########.fr       */
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
uint32_t	handle_empty_expansion(t_token *tokens, int i, char **ptr)
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
