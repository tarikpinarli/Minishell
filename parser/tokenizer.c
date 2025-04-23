/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:30:43 by tpinarli          #+#    #+#             */
/*   Updated: 2025/04/23 13:24:45 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_token	extract_quoted_token(char **str, char quote)
{
	const char	*start;
	size_t		len;
	t_token		token;

	(*str)++; // skip first quote
	start = *str;
	while (**str && **str != quote)
		(*str)++;

	len = *str - start;
	token.str = malloc(len + 1); // +2 tırnaklar için, +1 null
	if (!token.str)
	{
		token.str = NULL;
		token.quote = QUOTE_NONE;
		return (token);
	}
	ft_strncpy(token.str, start, len);
	token.str[len] = '\0';
	if (quote == '\'')
		token.quote = QUOTE_SINGLE;
	else
		token.quote = QUOTE_DOUBLE;
	if (**str == quote)
		(*str)++; // skip closing quote

	return (token);
}

t_token	extract_simple_token(char **str)
{
	char	*start;
	int		len;
	t_token	token;

	start = *str;
	while (**str && !ft_isspace(**str) && **str != '\'' && **str != '"')
		(*str)++;

	len = *str - start;
	token.str = malloc(len + 1);
	if (!token.str)
	{
		token.str = NULL;
		token.quote = QUOTE_NONE;
		return (token);
	}

	ft_strncpy(token.str, start, len);
	token.str[len] = '\0';
	token.quote = QUOTE_NONE;

	return (token);
}

t_token	next_token(char **str)
{
	t_token empty;
	
	while (**str && ft_isspace(**str))
		(*str)++;

	if (**str == '\'' || **str == '"')
		return (extract_quoted_token(str, **str));
	else if (**str)
		return (extract_simple_token(str));
	empty.str = NULL;
	empty.quote = QUOTE_NONE;
	return (empty);
}


t_token	*tokenize(char *input)
{
	int			i;
	int			count;
	t_token		*tokens;
	char		*ptr;

	if (!input)
		return (NULL);
	count = count_tokens(input);
	tokens = malloc(sizeof(t_token) * (count + 1));
	if (!tokens)
		return (NULL);
	ptr = input;
	i = 0;
	while (i < count)
	{
		tokens[i] = next_token(&ptr);
		if (!tokens[i].str)
		{
			while (--i >= 0)
				free(tokens[i].str);
			free(tokens);
			return (NULL);
		}
		i++;
	}
	tokens[count].str = NULL;
	tokens[count].quote = QUOTE_NONE;
	return tokens;
}
