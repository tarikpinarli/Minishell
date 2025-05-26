/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export_2.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 12:57:13 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/26 13:25:37 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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
		free(copy[i]);
		i++;
	}
	free(copy);
}

static void	sort_env_copy(char **copy)
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
	char	**copy;

	copy = copy_env(env);
	if (!copy)
		return ;
	sort_env_copy(copy);
	print_env(copy);
}
