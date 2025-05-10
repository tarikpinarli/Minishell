/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 13:45:04 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/10 13:36:19 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../minishell.h"

void    print_env(char **copy)
{
    int     i;
    char    *eq;
    
    i = 0;
    while (copy[i])
    {
        eq = ft_strchr(copy[i], '=');
        if (eq)
        {
            *eq = '\0';
            printf("declare -x %s=\"%s\"\n", copy[i], eq + 1);
            *eq = '=';
        }
        else
            printf("declare -x %s\n", copy[i]);
        free(copy[i]);
        i++;
    }
    free(copy);
}

void    sort_and_print_env(char **env)
{
    int     i;
    int     j;
    int     env_count;
    char    **copy;
    char    *temp;
    
    env_count = 0;
    while (env[env_count])
        env_count++;
    copy = malloc((env_count + 1) * sizeof(char *));
    if (!copy)
        return;
    i = 0;
    while (env[i])
    {
        copy[i] = ft_strdup(env[i]);
        i++;
    }
    copy[i] = NULL;
    i = 0;
    while (copy[i])
    {
        j = i + 1;
        while (copy[j])
        {
            if (ft_strcmp(copy[i], copy[j]) > 0)
            {
                temp = copy[i];
                copy[i] = copy[j];
                copy[j] = temp;    
            }
            j++;
        }
        i++;
    }
    print_env(copy);
}

int builtin_export(char **argv, int pid_flag, char **env)
{
    (void)argv;
    (void)pid_flag;

    if (!argv[1])
        sort_and_print_env(env);
    return (0);
}