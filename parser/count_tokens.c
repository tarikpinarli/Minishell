/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   count_tokens.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 13:07:24 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/11 20:25:09 by ykadosh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int		parse_single_token(const char *str, int *i, int *count);
static int8_t	index_through_quoted_token(const char *str, int *i);
static int		index_through_non_quoted_token(const char *str, int i);

/*
* return values: the number of tokens counted, or -1, in case the input string
* has an unclosed quotation mark within it
*/
int	count_tokens(const char *str)
{
	int		count;
	int		i;

	count = 0;
	i = 0;
	while (str[i])
	{
		while (str[i] && ft_isspace(str[i]))
			i++;
		if (parse_single_token(str, &i, &count) == -1)
			return (-1);
	}
	return (count);
}

/*
* Indexes through the token found in the string, according to the type of token
* it is (token in quotes, redirection characters, pipes, or a non quoted token).
* The count gets incremented accordingly from this function, since its address
* is passed as a parameter. The index i get incremented as well.
*
* returns -1 if an opening quotation mark does not have a pair;
* otherwise, return 0
*/
static int	parse_single_token(const char *str, int *i, int *count)
{
	if (str[*i] == '\'' || str[*i] == '"')
	{
		if (index_through_quoted_token(str, i) == -1)
		{
			printf("Quote not propery opened or closed.\n");
			return (-1);
		}
	}
	else if (str[*i] == '<' || str[*i] == '>')
	{
		(*i)++;
		if (str[*i] == str[*i - 1])
			(*i)++;
	}
	else if (str[*i] == '|')
		(*i)++;
	else if (str[*i])
		*i = index_through_non_quoted_token(str, *i);
	else
		return (0);
	(*count)++;
	return (0);
}

static int8_t	index_through_quoted_token(const char *str, int *i)
{
	char	quote;

	quote = str[*i];
	(*i)++;
	while (str[*i] && str[*i] != quote)
		(*i)++;
	if (str[*i] == quote)
		(*i)++;
	else
		return (-1);
	return (0);
}

static int	index_through_non_quoted_token(const char *str, int i)
{
	while (str[i] && !ft_isspace(str[i])
		&& str[i] != '\'' && str[i] != '"'
		&& str[i] != '<' && str[i] != '>' && str[i] != '|')
		i++;
	return (i);
}
