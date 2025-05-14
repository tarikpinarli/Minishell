/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_unset.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 17:32:05 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/14 17:36:40 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	builtin_unset(char **argv, char ***env)
{
	int	i;
	int	index;

	i = 1;
	while (argv[i])
	{
		index = var_exist(argv[i], *env);
		if (index >= 0)
			remove_env_var(env, index);
		i++;
	}
	return (0);
}
