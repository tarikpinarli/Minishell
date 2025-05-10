/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 13:45:04 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/10 15:41:14 by tpinarli         ###   ########.fr       */
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
//  Simple valid check
int valid_identifier(char *str)
{
    while (*str)
    {
        if (!ft_isalnum(*str) && *str != '=')
        {
            printf("%s: not a valid identifier\n", str);
            return (0);
        }
        str++;
    }
    return (1);
}

int builtin_export(char **argv, int pid_flag, char ***env)
{
    int     i;
    int     j;
    int     env_count;
    char    **copy;
    (void)pid_flag;

    i = 1;
    if (!argv[1])
    {
        sort_and_print_env(*env);
        return (0);
    }
    while (argv[i])
    {
        if (!valid_identifier(argv[i]))
            return (1);
        env_count = 0;
        while ((*env)[env_count])
            env_count++;
        copy = malloc(sizeof(char *) * (env_count + 2));
        if (!copy)
            return(1);
        j = 0;
        while ((*env)[j])
        {
            copy[j] = (*env)[j];
            j++;
        }
        copy[env_count++] = ft_strdup(argv[i]);
        copy[env_count] = NULL;
        (*env) = copy;
        i++;
    }
    return (0);
    /*i = 0;
    while ((*env)[i])
        i++;
    printf("env line coint: %d\n", i);
    copy = malloc(sizeof(char *) * (i + 2));
    if (!copy)
        return(0);
    i = 0;
    while ((*env)[i])
    {
        copy[i] = (*env)[i];
        i++;
    }
    copy[i] = ft_strdup("NEW VAR");
    i++;
    copy[i] = NULL;
    (*env) = copy;
    i = 0;
    while ((*env)[i])
    {
        printf("%s\n", (*env)[i]);
        i++;
    }
    return (0);*/
}