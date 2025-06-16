/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export_2.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 18:42:56 by tpinarli          #+#    #+#             */
/*   Updated: 2025/06/04 18:43:14 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	match_env_key(char *key, char **env)
{
	int		i;
	char	*eq;

	i = 0;
	while (env[i])
	{
		eq = ft_strchr(env[i], '=');
		if (eq)
			*eq = '\0';
		if (ft_strcmp(env[i], key) == 0)
		{
			if (eq)
				*eq = '=';
			return (i);
		}
		if (eq)
			*eq = '=';
		i++;
	}
	return (-1);
}

int	var_exist(char *arg, char **env)
{
	int		index;
	char	*eq;

	eq = ft_strchr(arg, '=');
	if (eq)
	{
		*eq = '\0';
		index = match_env_key(arg, env);
		*eq = '=';
		return (index);
	}
	else
		return (match_env_key(arg, env));
}

void	print_env(char **copy)
{
	int		i;
	char	*eq;

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
		i++;
	}
}

static void	sort_env(char **copy)
{
	int		i;
	int		j;
	char	*temp;

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
}

void	sort_and_print_env(char **env)
{
	sort_env(env);
	print_env(env);
}
