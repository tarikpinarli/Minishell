/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_echo.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:30:25 by tpinarli          #+#    #+#             */
/*   Updated: 2025/04/27 14:05:17 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int builtin_echo(char **argv)
{
    int i;
    int next_line;

    i = 1;
    next_line = 1;
    if (argv[1] && !strcmp(argv[1], "-n"))
    {
        i = 2;
        next_line = 0;
    }
    while (argv[i])
    {
        printf("%s", argv[i]);
        if (argv[i + 1])
            printf(" ");
        i++;
    }
    if (next_line)
        printf("\n");
    return (1);
}
