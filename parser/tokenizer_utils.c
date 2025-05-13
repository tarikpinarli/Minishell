/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykadosh <ykadosh@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/10 19:57:58 by ykadosh           #+#    #+#             */
/*   Updated: 2025/05/10 19:58:44 by ykadosh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static t_token	extract_quoted_token(char **str, char quote, int *line_id)
{
	const char	*start;
	size_t		len;
	t_token		token;

	ft_bzero(&token, sizeof(t_token));
	(*str)++;
	start = *str;
	while (**str && **str != quote)
		(*str)++;
	len = *str - start;
	token.str = malloc(len + 1);
	if (!token.str)
		return (token);
	(void)ft_strncpy(token.str, start, len);
	token.str[len] = '\0';
	if (quote == '\'')
		token.quote = QUOTE_SINGLE;
	else
		token.quote = QUOTE_DOUBLE;
	token.line_id = *line_id;
	if (**str == quote)
		(*str)++;
	return (token);
}

static t_token	extract_special_character_token(char **str, int *line_id)
{
	int		len;
	t_token	token;

	ft_bzero(&token, sizeof(t_token));
	len = 1;
	if ((**str == '<' || **str == '>') && (*(*str + 1) == **str))
		len++;
	token.str = ft_calloc(len + 1, sizeof(char));
	if (!token.str)
		return (token);
	(void)ft_memmove(token.str, *str, len);
	token.line_id = -1;
	(*str) += len;
	while (ft_isspace(**str))
		(*str)++;
	if (**str && **str != '<' && **str != '>' && **str != '|')
		(*line_id)++;
	return (token);
}

static t_token	extract_simple_token(char **str, int *line_id)
{
	char	*start;
	int		len;
	t_token	token;

	ft_bzero(&token, sizeof(t_token));
	start = *str;
	while (**str && !ft_isspace(**str)
		&& **str != '\'' && **str != '"'
		&& **str != '<' && **str != '>' && **str != '|')
		(*str)++;
	len = *str - start;
	token.str = malloc(len + 1);
	if (!token.str)
		return (token);
	(void)ft_strncpy(token.str, start, len);
	token.str[len] = '\0';
	token.quote = QUOTE_NONE;
	token.line_id = *line_id;
	return (token);
}

static t_token	next_token(char **str, int *line_id)
{
	t_token	empty;

	ft_bzero(&empty, sizeof(t_token));
	if (ft_isspace(**str))
	{
		while (**str && ft_isspace(**str))
			(*str)++;
		if (**str && **str != '<' && **str != '>' && **str != '|')
			(*line_id)++;
	}
	if (**str == '\'' || **str == '"')
		return (extract_quoted_token(str, **str, line_id));
	else if (**str == '<' || **str == '>' || **str == '|')
		return (extract_special_character_token(str, line_id));
	else if (**str)
		return (extract_simple_token(str, line_id));
	empty.str = NULL;
	empty.quote = QUOTE_NONE;
	return (empty);
}

int	fill_up_tokens(char *input, t_token **tokens, int count)
{
	static int	line_id;
	int			i;

	while (ft_isspace(*input))
		input++;
	i = 0;
	while (i < count)
	{
		(*tokens)[i] = next_token(&input, &line_id);
		if (!(*tokens)[i].str)
		{
			line_id = 0;
			return (-1);
		}
		i++;
	}
	line_id = 0;
	return (0);
}
