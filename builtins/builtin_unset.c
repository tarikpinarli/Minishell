/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_unset.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 17:32:05 by tpinarli          #+#    #+#             */
/*   Updated: 2025/06/04 15:30:28 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	builtin_unset(char **argv, char ***env) // pass cmd
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
