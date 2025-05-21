/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_tokens.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykadosh <ykadosh@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/11 20:37:00 by ykadosh           #+#    #+#             */
/*   Updated: 2025/05/11 20:44:55 by ykadosh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static uint32_t	is_expandable(const char *string);

void	expand_tokens(t_token *tokens, char *input)
{
	int			i;
	uint32_t	failure_flag;

	i = 0;
	failure_flag = 0;
	while (tokens[i].str)
	{
		if (tokens[i].quote == QUOTE_DOUBLE || tokens[i].quote == QUOTE_NONE)
		{
			if (is_expandable(tokens[i].str))
			{
				failure_flag = rebuild_expandable_string(tokens, i);
				if (failure_flag)
				{
					free_all(input, tokens, NULL);
					exit (last_exit_code(1, 1));
				}
			}
		}
		i++;
	}
}

/*
* returns true if the string passed as a parameter contains a '$' followed by
* either a '?' or a letter of the alphabet - otherwise, this program does not
* try and expand that string
*/
static uint32_t	is_expandable(const char *string)
{
	size_t	j;

	j = 0;
	while (string[j])
	{
		if (string[j] == '$'
			&& (string[j + 1] == '?'
				|| ft_isalpha(string[j + 1]) || string[j + 1] == '_'))
			return (1);
		j++;
	}
	return (0);
}
