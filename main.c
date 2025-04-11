/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:27:52 by tpinarli          #+#    #+#             */
/*   Updated: 2025/04/11 11:29:44 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int main(void)
{
    char *input;

    while (1)
    {
        setup_signals();
        input = readline("minishell$ ");
        if (!input)
            break;
        if (*input)
            add_history(input);
        printf("You typed: %s\n", input);
        free(input);
    }
    return 0;
}
