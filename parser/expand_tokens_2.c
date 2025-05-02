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

// This function needs to return a pointer, because we might have malloc()
// failures during cat_adjacent_strings - and probably in expand_variables,
// eventually. TODO: Should it be a double pointer?
void    expand_tokens(t_token *tokens, char *input, t_command *cmd)
{
    int     i;
    int32_t failure_flag;

    i = 0;
    failure_flag = 0;
    while (tokens[i].str)
    {
        printf("in expand_tokens(), tokens[%d].str is:  <%s>\n", i, tokens[i].str);
        if (tokens[i].quote == QUOTE_DOUBLE || tokens[i].quote == QUOTE_NONE)
        {
            failure_flag = expand_variables(tokens, i);
            if (failure_flag == -1)
            {
                free_all(input, tokens, cmd);
                exit (1);
            }
            printf("in expand_tokens(), tokens[%d].str is:  <%s>\n", i, tokens[i].str);
        /*
        if (!tokens[i].str)
        {
            // FIXME: This is not good enough. If we have a malloc() failure in
            // expand_variables(), even if you would return NULL from there to here,
            // tokens[i].str would then become NULL, and in the loop that indexes
            // through the non NULL tokens[i].str members within free_all()
            // to free each str, the freeing would stop at the current tokens[i].str,
            // which does not guarantee at all that further t_tokens.str would
            // not be freed at this point.
            // Try to free all the strings within expand_variables() if malloc()
            // fails
            free_all(input, tokens, cmd);
            exit (1);
        */
        }
        i++;
    }
}
