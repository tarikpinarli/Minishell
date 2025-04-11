/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 13:07:24 by tpinarli          #+#    #+#             */
/*   Updated: 2025/04/11 14:49:18 by tpinarli         ###   ########.fr       */
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
	int	count;
    char quote;
    
    count = 0;
	while (*str)
	{
		while (*str && ft_isspace(*str))
			str++;

		if (*str == '\'' || *str == '"')
		{
			quote = *str;
			str++; // Açılış tırnağını atla
			while (*str && *str != quote)
				str++;
			if (*str == quote)
				str++; // Kapanış tırnağını da atla
            else 
            {
                // In bash shel dquote something is happennig check it out.
                printf("QUOTE not propery closed. tokenizer_utils.c fix\n");
                exit (1);
            }
			count++;
		}
		else if (*str)
		{
			while (*str && !ft_isspace(*str) && *str != '\'' && *str != '"')
				str++;
			count++;
		}
	}
	return (count);
}

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
