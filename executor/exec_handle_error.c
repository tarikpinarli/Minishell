/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_handle_error.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 20:11:12 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/23 21:14:58 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	free_rest(char **path, t_command **cmd, char ***env)
{
	if (*path)
	{
		free(*path);
		*path = NULL;
	}
	if (*env)
		free_two_dimensional_array(env);
	if (*cmd)
		free_cmd(cmd);	
}

/* earlier iteration, not setting the pointers to NULL.
void	free_rest(char *path, t_command *cmd, char **env)
{
	if (path)
		free(path);
	if (env)
		free_2D_char(env);
	if (cmd)
		free_cmd(cmd);	
}
*/

void	handle_execve_error(char *command, char *path, t_command *cmd, char **env)
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
	perror(command);
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
