/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 13:45:04 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/09 14:00:57 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../minishell.h"

int builtin_export(char **argv, int pid_flag, char **env)
{
    (void)argv;
    (void)pid_flag;
    printf("%s\n", env[0]);
    printf("CONNECTED\n");
    return (0);
}