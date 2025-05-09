/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 12:48:37 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/09 16:27:23 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_numeric(const char *str)
{
	int	i;

	if (!str || *str == '\0')
		return (0);
	i = 0;
	if (str[i] == '-' || str[i] == '+')
		i++;
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	return (1);
}

int	builtin_exit(char **argv, t_command *cmd, int pid_flag)
{
	int	arg_count;
	int	exit_code;

	arg_count = 0;
	while (argv[arg_count])
		arg_count++;
	if (arg_count > 2)
	{
		ft_putendl_fd("exit: too many arguments", 2);
		return (1);
	}
	if (pid_flag)
		ft_putendl_fd("exit", 1);
	if (arg_count == 2 && !is_numeric(argv[1]))
	{
		ft_putstr_fd("exit: ", 2);
		ft_putstr_fd(argv[1], 2);
		ft_putendl_fd(": numeric argument required", 2);
		free_cmd(cmd);
		exit(2);
	}
	if (arg_count == 2)
		exit_code = ft_atoi(argv[1]);
	else
		exit_code = 0;

	free_cmd(cmd);
	exit(exit_code);
}



