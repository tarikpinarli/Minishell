/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_handle_error.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 20:11:12 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/26 14:16:03 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	command_not_found_err(t_command *cmd, char *path, char **env)
{
	ft_putstr_fd(cmd->argv[0], 2);
	ft_putendl_fd(": command not found", 2);
	free_rest(path, cmd, env);
	exit(127);
}

void	free_rest(char *path, t_command *cmd, char **env)
{
	if (path)
		free(path);
	if (env)
		free_two_dimention_char(env);
	if (cmd)
		free_cmd(cmd);
}

void	handle_execve_error(char *arg, char *path, t_command *cmd, char **env)
{
	if (errno == EISDIR)
	{
		ft_putstr_fd(path, 2);
		ft_putendl_fd(": Is a directory", 2);
		free_rest(path, cmd, env);
		exit(126);
	}
	else if (errno == EACCES)
	{
		ft_putstr_fd(path, 2);
		ft_putendl_fd(": Permission denied", 2);
		free_rest(path, cmd, env);
		exit(126);
	}
	else if (errno == ENOENT)
	{
		ft_putstr_fd(path, 2);
		ft_putendl_fd(": No such file or directory", 2);
		free_rest(path, cmd, env);
		exit(127);
	}
	perror(arg);
	exit(1);
}

void	check_if_directory(char *path, t_command *cmd, char **env)
{
	struct stat	st;

	if (stat(path, &st) == 0 && S_ISDIR(st.st_mode))
	{
		ft_putstr_fd(path, 2);
		ft_putendl_fd(": Is a directory", 2);
		free_rest(path, cmd, env);
		exit(126);
	}
}
