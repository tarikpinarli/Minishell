/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   count_tokens.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 13:07:24 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/10 19:01:52 by ykadosh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int		parse_single_token(const char *str, int *i, int *count);
static int8_t	index_through_quoted_token(const char *str, int *i);
static int		index_through_non_quoted_token(const char *str, int i);

/* FIXME: Delete this older, working but not refactored version when ready:
int	count_tokens(const char *str)
{
	int		count;
	char	quote;
	int		i;

	count = 0;
	i = 0;
	while (str[i])
	{
		while (str[i] && ft_isspace(str[i]))
			i++;
		if (str[i] == '\'' || str[i] == '"')
		{
			quote = str[i];
			i++; // skip opening quote
			while (str[i] && str[i] != quote)
				i++;
			if (str[i] == quote)
				i++; // skip closing quote
			else
			{
				// In bash shel dquote something is happennig check it out.
				// Update: no need to take care of that, enough to return error
				// if a quotation was left open.
				printf("Quote not propery opened or closed.\n");
				return (-1);
			}
			count++;
		}
		else if (str[i] == '<' || str[i] == '>')
		{
			i++;
			if (str[i] == str[i - 1])
				i++;
			count++;
		}
		
//		else if (str[i] == '<')
//		{
//			i++;
//			if (str[i] == '<')
//				i++;
//			count++;
//		}
//		else if (str[i] == '>')
//		{
//			i++;
//			if (str[i] == '>')
//				i++;
//			count++;
//		}
		
		else if (str[i] == '|')
		{
			i++;
			count++;
		}
		else if (str[i])
		{
			while (str[i] && !ft_isspace(str[i])
				&& str[i] != '\'' && str[i] != '"'
				&& str[i] != '<' && str[i] != '>' && str[i] != '|')
				i++;
			count++;
		}
	}
	return (count);
}
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

int	ft_isspace(char c)
{
	return (c == ' ' || c == '\t' || c == '\n'
		|| c == '\v' || c == '\f' || c == '\r');
}
