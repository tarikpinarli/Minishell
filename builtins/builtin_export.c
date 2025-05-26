/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 13:45:04 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/26 13:01:32 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int remove_env_var(char ***env, int index)
{
	int 	i;
	int		j;
	int		count;
	char	**new_env;

	count = 0;
	while ((*env)[count])
		count++;
	new_env = malloc(sizeof(char *) * count);
	if (!new_env)
		return (0);
	i = 0;
	j = 0;
	while (i < count)
	{
		if (i != index)
			new_env[j++] = ft_strdup((*env)[i]);
		free((*env)[i]);
		i++;
	}
	new_env[j] = NULL;
	free(*env);
	*env = new_env;
	return (1);
}


char **append_env_var(char *new_var, char **env)
{
	int		i;
	int		j;
	char	**new_env;

	i = 0;
	j = 0;
	while (env && env[i])
		i++;
	new_env = malloc(sizeof(char *) * (i + 2));
	if (!new_env)
		return (NULL);
	while (j < i)
	{
		new_env[j] = ft_strdup(env[j]);
		j++;
	}
	new_env[i] = ft_strdup(new_var);
	new_env[i + 1] = NULL;
	free_env(env);
	return new_env;
}


int	builtin_export(char **argv, int pid_flag, char ***env)
{
	int		i;
	int exist_index;
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
		exist_index = var_exist(argv[i], *env);
		if (exist_index >= 0)
		{
			remove_env_var(env, exist_index);
			*env = append_env_var(argv[i], *env);
		}
		else
			*env = append_env_var(argv[i], *env);
		i++;
	}
	return (0);
}
