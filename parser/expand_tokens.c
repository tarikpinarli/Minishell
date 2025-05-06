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


/*
// TEST:  these are leaking... UPDATE: these are NOT leaking anymore!!
// I suppose it might be happening when a variable does NOT expand, and there
// is no more text in the token's string after that varialbe AND there are
// other valid tokens in our *tokens array, we are facing an issue: the token
// might become NULL, and then, in the freeing step, we end up not freeing
// everything because we hit the NULL string too early...


$A  (this leaks a little bit later, probably with the 'cmd' functions. but the next cases seem to leak already here...)

"hello $" $$$$$ $A $12341

$A $USER $B $D

"echo " "hello $" $$$$$ $A $12341

$A $D $B $USER hello $R

FIXME: this case is not working !!!
jskldfjla ;jadn ;kdan ;nsa; knsdak fnnk n$USER $USER alkd;jalk;lasdnf; $USERfadlflaklssdfjlkaskj "$US"$US

FIXME: same, not working. any text after a valid variable just doesn't expand anymore....
$USERhello

$USER"hello"there$USER

*/

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
                    // TODO: add last_exit_code() call here!
                    exit (1);
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
