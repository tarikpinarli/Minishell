/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 13:45:04 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/13 14:31:28 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void print_env(char **copy)
{
    int i;
    char *eq;

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

void sort_and_print_env(char **env)
{
    int i, j, env_count;
    char **copy, *temp;

    env_count = 0;
    while (env[env_count])
        env_count++;
    copy = malloc(sizeof(char *) * (env_count + 1));
    if (!copy)
        return;
    for (i = 0; i < env_count; i++)
        copy[i] = ft_strdup(env[i]);
    copy[env_count] = NULL;

    for (i = 0; i < env_count - 1; i++)
    {
        for (j = i + 1; j < env_count; j++)
        {
            if (ft_strcmp(copy[i], copy[j]) > 0)
            {
                temp = copy[i];
                copy[i] = copy[j];
                copy[j] = temp;
            }
        }
    }
    print_env(copy);
}

int valid_identifier(char *str)
{
    int i = 0;

    if (!str || (!ft_isalpha(str[0]) && str[0] != '_'))
        return (0);
    while (str[i] && str[i] != '=')
    {
        if (!ft_isalnum(str[i]) && str[i] != '_')
            return (0);
        i++;
    }
    return (1);
}

int builtin_export(char **argv, int pid_flag, char ***env)
{
    int i = 1;
    int env_count;
    char *eq;
    int exists;

    (void)pid_flag;

    if (!argv[1])
    {
        sort_and_print_env(*env);
        return (0);
    }

    while (argv[i])
    {
        if (!valid_identifier(argv[i]))
        {
            fprintf(stderr, "export: `%s`: not a valid identifier\n", argv[i]);
            i++;
            continue;
        }

        eq = ft_strchr(argv[i], '=');
        env_count = 0;
        exists = 0;
        while ((*env)[env_count])
        {
            if (eq)
            {
                int name_len = eq - argv[i];
                if (!ft_strncmp((*env)[env_count], argv[i], name_len) && (*env)[env_count][name_len] == '=')
                    exists = 1;
            }
            else
            {
                if (!ft_strncmp((*env)[env_count], argv[i], ft_strlen(argv[i])) && (*env)[env_count][ft_strlen(argv[i])] == '=')
                    exists = 1;
            }
            env_count++;
        }

        char **new_env = malloc(sizeof(char *) * (env_count + (exists ? 1 : 2)));
        if (!new_env)
            return (1);

        for (int j = 0, k = 0; (*env)[j]; j++)
        {
            int skip = 0;
            if (exists)
            {
                int len = eq ? (eq - argv[i]) : ft_strlen(argv[i]);
                if (!ft_strncmp((*env)[j], argv[i], len) && (*env)[j][len] == '=')
                {
                    free((*env)[j]);
                    new_env[k++] = ft_strdup(argv[i]);
                    skip = 1;
                }
            }
            if (!skip)
                new_env[k++] = (*env)[j];
        }
        if (!exists)
            new_env[env_count] = ft_strdup(argv[i]);
        new_env[env_count + (exists ? 0 : 1)] = NULL;

        if (exists == 0)
            free(*env); // only if replacing full list
        *env = new_env;
        i++;
    }
    return (0);
}


