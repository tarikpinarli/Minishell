/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 12:48:37 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/07 13:40:41 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	builtin_exit(char **argv, t_command *cmd)
{
	int	arg_count;
	int	exit_code;

	arg_count = 0;
	while (argv[arg_count])
		arg_count++;
	if (arg_count > 2)
	{
		printf("exit: too many arguments\n");
		return (1);
	}
	printf("exit\n");
	if (arg_count == 2)
		exit_code = ft_atoi(argv[1]);
	else
		exit_code = 0;

	free_cmd(cmd);
	exit(exit_code);
	return (0);
}

