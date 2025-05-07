/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_tokens.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 15:58:09 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/05 18:41:32 by ykadosh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static uint32_t  is_expandable(const char *string);

void    expand_tokens(t_token *tokens, char *input, t_command *cmd)
{
    int     i;
    int32_t failure_flag;

    i = 0;
    failure_flag = 0;
    while (tokens[i].str)
    {
        if (tokens[i].quote == QUOTE_DOUBLE || tokens[i].quote == QUOTE_NONE)
        {
            if (is_expandable(tokens[i].str))
            {
                failure_flag = expand_variables(tokens, i);
                if (failure_flag)
                {
                    free_all(input, tokens, cmd);
                    exit (last_exit_code(1, 1));
                }
            }
        }
        i++;
    }
}

// WARN: Should we add here (and to the expand.c file) a check for an underscore?
/*
* returns true if the string passed as a parameter contains a '$' followed by
* either a '?' or a letter of the alphabet - otherwise, this program does not
* need to try and expand that string
*/
static uint32_t  is_expandable(const char *string)
{
    size_t  j;

    j = 0;
    while (string[j])
    {
        if (string[j] == '$' && (string[j + 1] == '?' || ft_isalpha(string[j + 1])))
            return (1);
        j++;
    }
    return (0);
}
