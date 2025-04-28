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

void expand_tokens(t_token *tokens)
{
    int     i;

    i = 0;
    while (tokens[i].str)
    {
        if (tokens[i].quote == QUOTE_DOUBLE || tokens[i].quote == QUOTE_NONE)
            tokens[i].str = expand_variables(tokens[i].str);
        i++;
    }
    // cat_strings
}

