/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_tokens.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/11 20:37:00 by ykadosh           #+#    #+#             */
/*   Updated: 2025/06/04 12:55:43 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static uint32_t	is_expandable(t_token *tokens, int *i);

void	expand_tokens(t_token *tokens, char *input, char ***env)
{
	int			i;
	uint32_t	failure_flag;

	i = 0;
	failure_flag = 0;
	while (tokens[i].str)
	{
		if (tokens[i].quote == QUOTE_DOUBLE || tokens[i].quote == QUOTE_NONE)
		{
			if (is_expandable(tokens, &i))
			{
				failure_flag = rebuild_expandable_string(env, tokens, i);
				if (failure_flag)
				{
					free_tokens_and_input(&tokens, &input);
					free_two_dimensional_array(env);
					rl_clear_history();
					write(2, ALLOCATION_FAILURE, sizeof(ALLOCATION_FAILURE) -1);
					exit (last_exit_code(1, 1));
				}
			}
		}
		i++;
	}
}

/*
 * yesterday's new version
static uint32_t	is_expandable(t_token *tokens, int *i)
{
	char	*s;
	int		j;

	if (*i > 0)
	{
		(*i)--;
		if (!ft_strcmp(tokens[*i].str, "<<"))
		{
			(*i)++;
			while (tokens[*i].line_id == tokens[*i + 1].line_id)
				(*i)++;
			return (0);
		}
		(*i)++;
	}
	s = tokens[*i].str;
	j = 0;
	while (s[j])
	{
		if (s[j] == '$'
			&& (s[j + 1] == '?' || ft_isalpha(s[j + 1]) || s[j + 1] == '_'))
			return (1);
		j++;
	}
	return (0);
}
*/

/*
 * WARN: add a comment regarding the quotes, after refactoring.
* returns true if the string passed as a parameter contains a '$' followed by
* '?', '_' or a letter of the alphabet - otherwise, this program does not try
* to expand that string. To mimick bash, which does not expand the heredoc's
* delimiter - even when it seems perfectly expandable in other scenarios - this
* function checks if the previous token is "<<". If it is, it parses through all
* the tokens which are to be merged (those sharing the same line_id with the
* current string token), incrementing i in the caller in order to avoid
* expanding that delimiter, and returns false.
*/
// NOTE: Most recent attempt, which also changes all quotes for heredoc delimiters!
static uint32_t	is_expandable(t_token *tokens, int *i)
{
	char	*s;
	int		j;
	int		quote_flag;

	if (*i > 0)
	{
		(*i)--;
		if (!ft_strcmp(tokens[*i].str, "<<"))
		{
			(*i)++;
			j = *i;
			quote_flag = 0;
			while (tokens[j + 1].str && tokens[j].line_id == tokens[j + 1].line_id)
			{
				if (tokens[j].quote == QUOTE_DOUBLE || tokens[j].quote == QUOTE_SINGLE
					|| tokens[j + 1].quote == QUOTE_DOUBLE || tokens[j + 1].quote == QUOTE_SINGLE)
					quote_flag = 1;
				j++;
			}
			if (j > *i && quote_flag)
			{
				while (*i < j)
				{
					tokens[*i].quote = QUOTE_DOUBLE;
					(*i)++;
				}
				tokens[*i].quote = QUOTE_DOUBLE;
			}
			*i = j;
			return (0);
		}
		(*i)++;
	}
	s = tokens[*i].str;
	j = 0;
	while (s[j])
	{
		if (s[j] == '$'
			&& (s[j + 1] == '?' || ft_isalpha(s[j + 1]) || s[j + 1] == '_'))
			return (1);
		j++;
	}
	return (0);
}
