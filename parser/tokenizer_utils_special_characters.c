/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_utils_special_characters.c               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 13:07:24 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/09 11:56:34 by ykadosh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	ft_isspace(char c)
{
	return (c == ' ' || c == '\t' || c == '\n'
		|| c == '\v' || c == '\f' || c == '\r');
}

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
		else if (str[i] == '<')
		{
			i++;
			if (str[i] == '<')
				i++;
			count++;
		}
		else if (str[i] == '>')
		{
			i++;
			if (str[i] == '>')
				i++;
			count++;
		}
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

/* WARN: are we gonna use this, for refactoring?
static uint8_t	is_pipe_or_redirection(char c)
{
	if (c == '<' || c == '>' || c == '|')
		return (1);
	return (0);
}
*/

char	*ft_strncpy(char *dst, const char *src, size_t n)
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
