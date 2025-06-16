/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 12:48:37 by tpinarli          #+#    #+#             */
/*   Updated: 2025/06/16 15:30:33 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

long	ft_atol(const char *str)
{
	int		i;
	long		sign;
	long		result;

	i = 0;
	sign = 1;
	result = 0;
	while ((str[i] >= '\t' && str[i] <= '\r') || str[i] == ' ')
		i++;
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i] == '-')
			sign = -1;
		i++;
	}
	while (str[i] >= '0' && str[i] <= '9')
	{
		if (sign == -1 && (result * sign > 0))
			return (0);
		if (sign == 1 && (result * sign < 0))
			return (-1);
		result = result * 10 + (str[i] - '0');
		i++;
	}
	return (result * sign);
}

uint64_t	is_too_big(const char *str)
{
	uint64_t	num;
	int				isneg;
	int				i;

	num = 0;
	isneg = 1;
	i = 0;
	while (ft_isspace(str[i]))
		i++;
	if (str[i] == '+' || str[i] == '-')
	{
		if (str[i] == '-')
			isneg *= -1;
		i++;
	}
	while (str[i] >= '0' && str[i] <= '9')
	{
		num = (num * 10) + (str[i] - '0');
		if (num == (uint64_t)LONG_MAX + 1 && isneg > 0)
			return (1);
		if (num * (-1) < 0)
			return(1);
		i++;
	}
	return (0);
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
	free_two_dimensional_array(&env);
	free_cmd(&cmd);
	close(3);
	close(4);
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
}

int	builtin_exit(char **argv, t_command *cmd, int pid_flag, char ***env)
{
	int		arg_count;
	long	exit_code;

	arg_count = 0;
	printf("%ld\n", LONG_MAX);
	printf("%ld\n", LONG_MIN);
	while (argv[arg_count])
		arg_count++;
	if (arg_count > 2)
		return (ft_putendl_fd("exit\nexit: too many arguments", 2), 1);
	if (pid_flag)
		ft_putendl_fd("exit", 1);
	if (arg_count >= 2 && (!is_numeric(argv[1]) || is_too_big(argv[1])))
	{
		ft_putstr_fd("exit: ", 2);
		ft_putstr_fd(argv[1], 2);
		ft_putendl_fd(": numeric argument required", 2);
		close_and_free(*env, cmd);
		exit(2);
	}
	if (arg_count == 2)
		exit_code = ft_atol(argv[1]);
	else
		exit_code = last_exit_code(0, 0);
	close_and_free(*env, cmd);
	exit(exit_code);
}
