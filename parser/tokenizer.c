/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:30:43 by tpinarli          #+#    #+#             */
/*   Updated: 2025/04/11 14:30:13 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	*extract_quoted_token(const char **str, char quote)
{
	const char	*start;
	size_t		len;
	char		*token;

	(*str)++; // ilk tırnağı atla
	start = *str;

	while (**str && **str != quote)
		(*str)++;

	// Eğer kapanan tırnak yoksa -> syntax error gibi davran
	if (**str != quote)
		return (NULL); // veya özel hata mesajı üret

	len = *str - start;
	token = malloc(len + 3); // +2 tırnaklar için, +1 null
	if (!token)
		return (NULL);

	token[0] = quote;
	ft_strncpy(token + 1, start, len);
	token[len + 1] = quote;
	token[len + 2] = '\0';

    if (**str == quote)
	    (*str)++; // kapanan tırnağı atla

	return (token);
}


static char	*extract_simple_token(const char **str)
{
	const char	*start = *str;
	size_t		len;
	char		*token;

	while (**str && !ft_isspace(**str) && **str != '\'' && **str != '"')
		(*str)++;

	len = *str - start;
	token = malloc(len + 1);
	if (!token)
		return (NULL);

	ft_strncpy(token, start, len);
	token[len] = '\0';

	return (token);
}

static char	*next_token(const char **str)
{
	while (**str && ft_isspace(**str))
		(*str)++;

	if (**str == '\'' || **str == '"')
		return extract_quoted_token(str, **str);
	else if (**str)
		return extract_simple_token(str);

	return NULL;
}


char	**tokenize(const char *input)
{
    int         i;
    int         count;
    char        **tokens;
    const char  *ptr;
    
    if (!input)
        return (NULL);
    count = count_tokens(input);
    tokens = malloc(sizeof(char *) * (count + 1));
    if (!tokens)
        return (NULL);
    ptr = input;
    i = 0;
    while (i < count)
    {
        tokens[i] = next_token(&ptr);
        if (!tokens[i])
        {
            while (--i >= 0)
                free(tokens[i]);
            free(tokens);
            return (NULL);
        }
        i++;
    }
    tokens[count] = NULL;
    return (tokens);
}
