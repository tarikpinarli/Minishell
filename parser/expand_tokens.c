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
static void		avoid_heredoc_delimiter_expansion(t_token *tokens, int *i);

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

// TODO: comments!! checks whether the string at 
static uint32_t	is_expandable(t_token *tokens, int *i)
{
	if (*i > 0 && !ft_strcmp(tokens[*i - 1].str, "<<"))
	{
		avoid_heredoc_delimiter_expansion(tokens, i);
		return (0);
	}
	return (check_if_str_contains_vars_to_expand(tokens[*i].str));
}

/*
* This function is to be called when Minishell's input contains a "<<" token,
* which is intended for using a temporary heredoc file. Since the heredoc
* delimiter (which follows the "<<" token) never gets expanded by bash, this
* function skips all tokens which are to be merged, incrementing the pointer
* to index 'i', so that the caller will not try to expand those.
* Furthermore, if one of those tokens contains quotes, whether double or single,
* all of those tokens' quote values will be converted to QUOTE_DOUBLE: the
* reason for this is that on bash, if the heredoc's delimiter contains single or
* double quotes in its 'mergeable' tokens, then bash does NOT expand the
* dollar sign variables inputted into the heredoc file; If there are no quotes
* however, the delimiter tokens' quote value will be zero, telling Minishell
* to expand those variables.
*/
static void	avoid_heredoc_delimiter_expansion(t_token *tokens, int *i)
{
	int	quote_flag;
	int	j;

	quote_flag = 0;
	j = *i;
	while (tokens[j + 1].str && tokens[j].line_id == tokens[j + 1].line_id)
	{
		if (tokens[j].quote == QUOTE_DOUBLE
			|| tokens[j].quote == QUOTE_SINGLE
			|| tokens[j + 1].quote == QUOTE_DOUBLE
			|| tokens[j + 1].quote == QUOTE_SINGLE)
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
		*i = j;
	}
}

// TODO: comments!
uint32_t	check_if_str_contains_vars_to_expand(char *string)
{
	int	j;
	
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
