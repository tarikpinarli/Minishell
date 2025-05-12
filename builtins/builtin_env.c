/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_env.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 16:30:38 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/10 15:15:08 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int char_exist(char *str)
{
    int i;

    i = 0;
    while (str[i])
    {
        if (str[i] == '=')
            return (1);
        i++;
    }
    return(0);
}

int    builtin_env(char ***env)
{
    int i;

    i = 0;
    while ((*env)[i])
    {
        if (char_exist((*env)[i]))
            printf("%s\n", (*env)[i]);
        i++;
    }
    return (0);
}