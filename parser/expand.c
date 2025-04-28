/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/12 14:29:13 by tpinarli          #+#    #+#             */
/*   Updated: 2025/04/28 16:48:13 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*expand_variables(const char *str)
{
	const char	*ptr = str;
	char		*result = ft_calloc(1, 1); // boş başlangıç
	char		*varname;
	char		*value;
	char		*before;
	char 		*exit_str;
	size_t		len;

	while (*ptr)
	{
        if (*ptr == '$' && *(ptr + 1) == '?')
        {
            exit_str = ft_itoa(last_exit_code(0, 0));
            before = result;
            result = ft_strjoin(result, exit_str);
            free(before);
            free(exit_str);
            ptr += 2;
            continue;
        }
		else if (*ptr == '$' && ft_isalpha(*(ptr + 1)))
		{
			ptr++; // skip $ sign
			len = 0;
			while (ft_isalnum(ptr[len]) || ptr[len] == '_')
				len++;

			varname = ft_substr(ptr, 0, len);
			value = getenv(varname);
			free(varname);

			if (value)
			{
				before = result;
				result = ft_strjoin(result, value);
				free(before);
			}
			ptr += len;
		}
		else
		{
			char tmp[2] = {*ptr, '\0'};
			before = result;
			result = ft_strjoin(result, tmp);
			free(before);
			ptr++;
		}
	}
	return result;
}
