/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:27:52 by tpinarli          #+#    #+#             */
/*   Updated: 2025/04/11 12:11:40 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int main(void)
{
    char *input;

    rl_catch_signals = 0;
    setup_signals();
    while (1)
    {
        input = readline("minishell$ ");
        if (!input) // If user input Ctrl+D 
            break;
        if (*input)
            add_history(input);
        printf("You typed: %s\n", input);
        free(input);
    }
    return 0;
}
