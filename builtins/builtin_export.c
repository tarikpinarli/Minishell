/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 13:45:04 by tpinarli          #+#    #+#             */
/*   Updated: 2025/06/04 19:26:22 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	*replace_env_var(char **env, char *new_var, int exist_index)
{
	char	*temp;

	temp = NULL;
	free(env[exist_index]);
	env[exist_index] = NULL;
	temp = ft_strdup(new_var);
	return (temp);
}

char	**append_env_var(char *new_var, char **env)
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
		if (!new_env[j])
			return (free_two_dimensional_array(&new_env), NULL);
		j++;
	}
	new_env[i] = ft_strdup(new_var);
	if (!new_env[i])
		return (free_two_dimensional_array(&new_env), NULL);
	new_env[i + 1] = NULL;
	free_two_dimensional_array(&env);
	return (new_env);
}

int	handle_env_var(char *arg, char ***env)
{
	int		exist_index;
	char	**temp;

	exist_index = var_exist(arg, *env);
	if (exist_index >= 0)
	{
		(*env)[exist_index] = replace_env_var(*env, arg, exist_index);
		if (!(*env)[exist_index])
			return (-1);
	}
	else
	{
		temp = append_env_var(arg, *env);
		if (!temp)
			return (-1);
		*env = temp;
	}
	return (0);
}

int	process_export_args(char **argv, char ***env)
{
	int	i;

	i = 1;
	while (argv[i])
	{
		if (!valid_identifier(argv[i]))
			return (1);
		if (handle_env_var(argv[i], env) < 0)
			return (-1);
		i++;
	}
	return (0);
}

int	builtin_export(char **argv, char ***env)
{
	int		ret;

	if (!argv[1])
	{
		sort_and_print_env(*env);
		return (0);
	}
	ret = process_export_args(argv, env);
	return (ret);
}
