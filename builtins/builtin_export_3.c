/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export_3.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 12:58:44 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/26 13:25:24 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	match_env_var(char *arg, char **env, int i, char *eq)
{
	char	*eq_2;

	while (env[i])
	{
		eq_2 = ft_strchr(env[i], '=');
		if (eq_2)
		{
			*eq_2 = '\0';
			if (!ft_strcmp(env[i], arg))
			{
				*eq = '=';
				*eq_2 = '=';
				return (i);
			}
			*eq_2 = '=';
		}
		i++;
	}
	*eq = '=';
	return (-1);
}

static int	match_env_key_only(char *head, char **env)
{
	int		i;
	char	*eq_2;

	i = 0;
	while (env[i])
	{
		eq_2 = ft_strchr(env[i], '=');
		if (eq_2)
		{
			*eq_2 = '\0';
			if (!strcmp(env[i], head))
			{
				*eq_2 = '=';
				return (i);
			}
			*eq_2 = '=';
		}
		i++;
	}
	return (-1);
}

int	var_exist(char *arg, char **env)
{
	char	*eq;

	eq = strchr(arg, '=');
	if (eq)
		return (match_env_var(arg, env, 0, eq));
	else
		return (match_env_key_only(arg, env));
}
