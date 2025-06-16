/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_unset.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 17:32:05 by tpinarli          #+#    #+#             */
/*   Updated: 2025/06/04 19:25:36 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	find_and_remove(char ***env, int count, int index, char **new_env)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (i < count)
	{
		if (i != index)
		{
			new_env[j++] = ft_strdup((*env)[i]);
			if (!new_env[j - 1])
			{
				free_two_dimensional_array(&new_env);
				return (0);
			}
		}
		i++;
	}
	new_env[j] = NULL;
	return (1);
}

static int	remove_env_var(char ***env, int index)
{
	int		ret;
	int		count;
	char	**new_env;

	count = 0;
	while ((*env)[count])
		count++;
	new_env = malloc(sizeof(char *) * count);
	if (!new_env)
		return (0);
	ret = find_and_remove(env, count, index, new_env);
	if (ret)
	{
		free_two_dimensional_array(env);
		*env = new_env;
	}
	else
		free_two_dimensional_array(&new_env);
	return (ret);
}

int	builtin_unset(char **argv, char ***env)
{
	int	i;
	int	index;

	i = 1;
	while (argv[i])
	{
		index = var_exist(argv[i], *env);
		if (index >= 0)
		{
			if (!remove_env_var(env, index))
				return (-1);
		}
		i++;
	}
	return (0);
}
