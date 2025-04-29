/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_tokens.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 15:58:09 by tpinarli          #+#    #+#             */
/*   Updated: 2025/04/28 17:50:13 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// This function needs to return a pointer, because we might have malloc()
// failures during cat_adjacent_strings - and probably in expand_variables,
// eventually. TODO: Should it be a double pointer?
void    expand_tokens(t_token *tokens)
{
    int     i;

    i = 0;
    while (tokens[i].str)
    {
        printf("in expand_tokens(), tokens[%d].str is:  <%s>\n", i, tokens[i].str);
        if (tokens[i].quote == QUOTE_DOUBLE || tokens[i].quote == QUOTE_NONE)
            tokens[i].str = expand_variables(tokens[i].str);
        i++;
    }
    /*
    if (!cat_adjacent_strings(tokens, tokens[i - 1].line_id));
        return (NULL);
    // NOTE: it is not enough to return NULL; we need to free everything somewhere!
    return ( ??? );
    */
}
