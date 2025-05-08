/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_tokens.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 15:58:09 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/08 17:09:49 by ykadosh          ###   ########.fr       */
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
                failure_flag = rebuild_string(tokens, i);
                if (failure_flag)
                {
                    free_all(input, tokens, cmd);
                    exit (last_exit_code(1, 1));
                }
            }
        }
        i++;
    }
    i = 0;
    printf("in expand_tokens, at the very end of the loop, the tokens are:\n");
    while (tokens[i].str)
    {
        printf("token[%d]: <%s>	---- quote type:	<%d>	---- line_id:	<%d>"
            "--- length:	<%zu>\n",
            i, tokens[i].str, tokens[i].quote, tokens[i].line_id,
            ft_strlen(tokens[i].str));
        i++;
    }
    printf("\n\n");
}

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
        if (string[j] == '$' &&
            (string[j + 1] == '?'
            || ft_isalpha(string[j + 1]) || string[j + 1] == '_'))
            return (1);
        j++;
    }
    return (0);
}
