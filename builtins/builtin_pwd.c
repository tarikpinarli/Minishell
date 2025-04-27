/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_pwd.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 15:00:56 by tpinarli          #+#    #+#             */
/*   Updated: 2025/04/27 14:13:27 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int builtin_pwd()
{
    char    buf[PATH_MAX];
    char    *ptr;

    ptr = getcwd(buf, PATH_MAX);
    if (ptr)
    {
        printf("%s\n", ptr);
        return (EXIT_SUCCESS);
    }
    return (EXIT_FAILURE);
}