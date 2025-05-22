/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 12:48:37 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/22 18:45:52 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int is_too_big(const char *str)
{
	int len;
	int i;

	len = 0;
	i = 0;
	if (!str)
		return (1);
	if (str[0] == '-' || str[0] == '+')
		i++;
	while (str[i])
	{
		len++;
		i++;
	}
	return (len > 19);
}

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

void	close_and_free(char **env, t_command *cmd)
{
	rl_clear_history();
	free_env(env);
	free_cmd(cmd);
	close(3);
	close(4);
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
}

int	builtin_exit(char **argv, t_command *cmd, int pid_flag, char ***env)
{
	int	arg_count;
	int	exit_code;

	arg_count = 0;
	while (argv[arg_count])
		arg_count++;
	if (arg_count > 2)
		return (ft_putendl_fd("exit: too many arguments", 2), 1);
	if (pid_flag)
		ft_putendl_fd("exit", 1);
	if (arg_count >= 2 && (!is_numeric(argv[1]) || is_too_big(argv[1])))
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
	close_and_free(*env, cmd);
	exit(exit_code);
}
