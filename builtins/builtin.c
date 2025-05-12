/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/27 13:27:12 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/10 14:09:52 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int is_builtin(char *cmd)
{
	if (!cmd)
		return (0);
	return (!ft_strcmp(cmd, "cd")
		|| !ft_strcmp(cmd, "pwd")
		|| !ft_strcmp(cmd, "export")
		|| !ft_strcmp(cmd, "unset")
		|| !ft_strcmp(cmd, "env")
		|| !ft_strcmp(cmd, "exit")
		|| !ft_strcmp(cmd, "echo"));
}

int execute_builtin(t_command *cmd, int pid_flag, char ***env)
{
	if (!ft_strcmp(cmd->argv[0], "pwd"))
		return (builtin_pwd());
    else if (!ft_strcmp(cmd->argv[0], "echo"))
		return (builtin_echo(cmd->argv));
	//else if (!ft_strcmp(cmd->argv[0], "cd"))
	//	return (builtin_cd(cmd->argv));
	else if (!ft_strcmp(cmd->argv[0], "export"))
		return (builtin_export(cmd->argv, pid_flag, env));
	//else if (!ft_strcmp(cmd->argv[0], "unset"))
	//	return (builtin_unset(cmd->argv));
	else if (!ft_strcmp(cmd->argv[0], "env"))
		return (builtin_env(env));
	else if (!ft_strcmp(cmd->argv[0], "exit"))
		return (builtin_exit(cmd->argv, cmd, pid_flag));
	return (1);
}

