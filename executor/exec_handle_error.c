/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_handle_error.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 20:11:12 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/22 20:33:29 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	handle_execve_error(char *cmd, char *path)
{
	if (errno == EISDIR)
	{
		ft_putstr_fd(path, 2);
		ft_putendl_fd(": Is a directory", 2);
		exit(126);
	}
	else if (errno == EACCES)
	{
		ft_putstr_fd(path, 2);
		ft_putendl_fd(": Permission denied", 2);
		exit(126);
	}
	else if (errno == ENOENT)
	{
		ft_putstr_fd(path, 2);
		ft_putendl_fd(": No such file or directory", 2);
		exit(127);
	}
	else
	{
		perror(cmd);
		exit(1);
	}
}

void	check_if_directory(char *path)
{
	struct stat	st;

	if (stat(path, &st) == 0 && S_ISDIR(st.st_mode))
	{
		ft_putstr_fd(path, 2);
		ft_putendl_fd(": Is a directory", 2);
		exit(126);
	}
}
