/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_tokens_2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 15:58:09 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/02 14:19:40 by ykadosh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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
            failure_flag = expand_variables(tokens, i);
            if (failure_flag == -1)
            {
                free_all(input, tokens, cmd);
                exit (1);
            }
        }
        i++;
    }
}
