/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:30:43 by tpinarli          #+#    #+#             */
/*   Updated: 2025/04/12 14:04:16 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static t_token	extract_quoted_token(const char **str, char quote)
{
	const char	*start;
	size_t		len;
	t_token		token;

	(*str)++; // ilk tırnağı atla
	start = *str;

	while (**str && **str != quote)
		(*str)++;

	len = *str - start;

	token.str = malloc(len + 3); // +2 tırnaklar için, +1 null
	if (!token.str)
	{
		token.str = NULL;
		token.quote = QUOTE_NONE;
		return (token);
	}

	token.str[0] = quote;
	ft_strncpy(token.str + 1, start, len);
	token.str[len + 1] = quote;
	token.str[len + 2] = '\0';
	token.quote = (quote == '\'') ? QUOTE_SINGLE : QUOTE_DOUBLE;

	if (**str == quote)
		(*str)++; // kapanan tırnağı atla

	return (token);
}

static t_token	extract_simple_token(const char **str)
{
	const char	*start = *str;
	size_t		len;
	t_token		token;

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

static t_token	next_token(const char **str)
{
	while (**str && ft_isspace(**str))
		(*str)++;

	if (**str == '\'' || **str == '"')
		return extract_quoted_token(str, **str);
	else if (**str)
		return extract_simple_token(str);

	t_token empty = {NULL, QUOTE_NONE};
	return empty;
}


t_token	*tokenize(const char *input)
{
	int			i;
	int			count;
	t_token		*tokens;
	const char	*ptr;

	if (!input)
		return (NULL);
	count = count_tokens(input); // bunu şimdilik aynı bırakabiliriz
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
