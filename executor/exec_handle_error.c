/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_handle_error.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 20:11:12 by tpinarli          #+#    #+#             */
/*   Updated: 2025/06/08 11:56:13 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	free_rest(char **path, t_command **cmd, char ***env)
{
	if (path && *path)
	{
		free(*path);
		*path = NULL;
	}
	if (env && *env)
		free_two_dimensional_array(env);
	if (cmd && *cmd)
		free_cmd(cmd);
}

void	handle_execve_error(char *str, char **path, t_command **cmd,
			char ***env)
{
	if (errno == EISDIR)
	{
		ft_putstr_fd(*path, 2);
		ft_putendl_fd(": Is a directory", 2);
		free_rest(path, cmd, env);
		exit(126);
	}
	else if (errno == EACCES)
	{
		ft_putstr_fd(*path, 2);
		ft_putendl_fd(": Permission denied", 2);
		free_rest(path, cmd, env);
		exit(126);
	}
	else if (errno == ENOENT)
	{
		ft_putstr_fd(*path, 2);
		ft_putendl_fd(": No such file or directory", 2);
		free_rest(path, cmd, env);
		exit(127);
	}
	perror(str);
	free_rest(path, cmd, env);
	exit(1);
}

void	check_if_directory(char **path, t_command **cmd, char ***env)
{
	struct stat	st;

	if (stat(*path, &st) == 0 && S_ISDIR(st.st_mode))
	{
		ft_putstr_fd(*path, 2);
		ft_putendl_fd(": Is a directory", 2);
		free_rest(path, cmd, env);
		exit(126);
	}
}
