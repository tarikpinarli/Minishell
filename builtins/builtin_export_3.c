/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export_3.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 18:39:22 by tpinarli          #+#    #+#             */
/*   Updated: 2025/06/04 18:41:44 by tpinarli         ###   ########.fr       */
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
