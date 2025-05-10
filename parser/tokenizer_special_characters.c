/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_special_characters.c                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:30:43 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/10 17:50:14 by ykadosh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	*ft_strncpy(char *dst, const char *src, size_t n)
{
	size_t	i;

	i = 0;
	while (i < n && src[i])
	{
		dst[i] = src[i];
		i++;
	}
	while (i < n)
	{
		dst[i] = '\0';
		i++;
	}
	return (dst);
}

static t_token	extract_quoted_token(char **str, char quote, int *line_id)
{
	const char	*start;
	size_t		len;
	t_token		token;

	ft_bzero(&token, sizeof(t_token));
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
	token.line_id = *line_id;
	if (**str == quote)
		(*str)++; // skip closing quote

	return (token);
}

static t_token	extract_simple_token(char **str, int *line_id)
{
	char	*start;
	int		len;
	t_token	token;

	ft_bzero(&token, sizeof(t_token));
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
	token.line_id = *line_id;
	return (token);
}

static t_token	next_token(char **str, int *line_id)
{
	t_token		empty;
	
	ft_bzero(&empty, sizeof(t_token));
	if (ft_isspace(**str))
	{
		(*line_id)++;
		while (**str && ft_isspace(**str))
			(*str)++;
	}
	if (**str == '\'' || **str == '"')
		return (extract_quoted_token(str, **str, line_id));
	else if (**str)
		return (extract_simple_token(str, line_id));
	empty.str = NULL;
	empty.quote = QUOTE_NONE;
	return (empty);
}

/*
* return values:
* ◦ positive value: if everything went smoothly, this function returns the count
* obtained by the call to count_tokens().
* ◦ -1: if a quotation mark has not been closed in the input.
* ◦ -2: on malloc failure
*/
int	tokenize(char *input, t_token **tokens)
{
	int			i;
	int			count;
	static int	line_id;

	/*
	 * FIXME: Now trying without this, as it is probably unnecessary.
	if (!input)
		return (-3);
	*/
	count = count_tokens(input);
	if (count < 0)
	{
		free(input);
		return (-1); // same as the return value of count_tokens()
	}
	printf("TOKENS COUNTED :	<%d>\n\n\n", count); // WARN: just printf() debugging
	*tokens = ft_calloc((count + 1), sizeof(t_token));
	if (!*tokens)
	{
		free(input);
		return (-2);
	}
	while (ft_isspace(*input))
		input++;
	i = 0;
	while (i < count)
	{
		(*tokens)[i] = next_token(&input, &line_id);
		if (!(*tokens)[i].str) // TODO: can we call free_all() from here, instead?
		{
			while (--i >= 0)
				free((*tokens)[i].str);
			free(*tokens);
			*tokens = NULL;
			line_id = 0;
			free(input);
			return (-2);
		}
		i++;
	}
	line_id = 0;
	(*tokens)[count].str = NULL;
	(*tokens)[count].quote = QUOTE_NONE;
	return (count);
}
