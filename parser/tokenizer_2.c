/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:30:43 by tpinarli          #+#    #+#             */
/*   Updated: 2025/04/27 17:07:35 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static t_token	extract_quoted_token(char **str, char quote, int *line_id)
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
 * older version: modified in order to avoid leaks and handle more cases!
t_token	*tokenize(char *input)
{
	int			i;
	int			count;
	t_token		*tokens;
	static int	line_id;

	if (!input)
		return (NULL);
	count = count_tokens(input);
	if (count < 0)
		return (NULL);
	tokens = malloc(sizeof(t_token) * (count + 1));
	if (!tokens)
		return (NULL);
	while (ft_isspace(*input))
		input++;
	i = 0;
	while (i < count)
	{
		tokens[i] = next_token(&input, &line_id);
		if (!tokens[i].str)
		{
			while (--i >= 0)
				free(tokens[i].str);
			free(tokens);
			line_id = 0;
			return (NULL);
		}
		i++;
	}
	line_id = 0;
	tokens[count].str = NULL;
	tokens[count].quote = QUOTE_NONE;
	return (tokens);
}
*/

/*
* return values:
* ◦ positive value: if everything went smoothly, this function returns the count
* obtained by the call to count_tokens().
* ◦ -1: if a quotation mark has not been closed in the input.
* ◦ -2: on malloc failure
* ◦ -3: if the input passed to this function is NULL. WARN: unnecessary check?
*/
int	tokenize(char *input, t_token **tokens)
{
	int			i;
	int			count;
	static int	line_id;

	if (!input)
		return (-3);
// TODO: Ask Tarik if input can really be NULL when arriving here?
// There is already a check in main() for (!input) before calling tokenize().
// Or is this related to something regarding signals?
	count = count_tokens(input);
	if (count < 0)
	{
		free(input);
		return (-1); // same as the return value of count_tokens()
	}
	*tokens = malloc(sizeof(t_token) * (count + 1));
	if (!*tokens)
		return (-2);
	while (ft_isspace(*input))
		input++;
	i = 0;
	while (i < count)
	{
		(*tokens)[i] = next_token(&input, &line_id);
		if (!(*tokens)[i].str)
		{
			while (--i >= 0)
				free((*tokens)[i].str);
			free(*tokens);
			*tokens = NULL;
			line_id = 0;
			return (-2);
		}
		i++;
	}
	line_id = 0;
	(*tokens)[count].str = NULL;
	(*tokens)[count].quote = QUOTE_NONE;
	return (count);
}
