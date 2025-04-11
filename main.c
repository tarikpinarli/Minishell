/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:27:52 by tpinarli          #+#    #+#             */
/*   Updated: 2025/04/11 15:44:34 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int main(void)
{
    char        *input;
    char        **tokens;
    t_command   *cmd;

    rl_catch_signals = 0;
    setup_signals();
    while (1)
    {
        input = readline("minishell$ ");
        if (!input) // If user input Ctrl+D
        {
            rl_clear_history();
            printf("exit\n");
            break;
        }
        if (*input) // If user types anything but enter
            add_history(input);
        tokens = tokenize(input);

        cmd = parse_tokens(tokens);
        for (int i = 0; cmd->args[i]; i++)
            printf("arg[%d]: %s\n", i, cmd->args[i]);
        if (cmd->outfile)
            printf("outfile: %s (append = %d)\n", cmd->outfile, cmd->append);


        // Free tokens
        if (tokens)
        {
            for (int i = 0; tokens[i]; i++)
                free(tokens[i]);
            free(tokens);
        }
        free(input);
    }
    rl_clear_history();
    return 0;
}
