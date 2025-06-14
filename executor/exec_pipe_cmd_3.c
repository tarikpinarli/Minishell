/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipe_cmd_3.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykadosh <ykadosh@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/10 23:30:06 by ykadosh           #+#    #+#             */
/*   Updated: 2025/06/10 23:37:37 by ykadosh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
 * Frees all allocated resources and exits the process with the given exit code.
 * Used in child processes when an error occurs and cleanup is required.
 */
void	free_and_exit(char **path, t_command **cmd, char ***env,
			int exit_status)
{
	free_rest(path, cmd, env);
	exit (exit_status);
}

static void	check_for_missing_cmd_and_handle(char **path, char ***env,
				t_command **cmd, t_command *current)
{
	if (!current->argv)
		free_and_exit(path, cmd, env, 0);
	if (!current->argv[0] || !current->argv[0][0])
	{
		ft_putendl_fd("Command '' not found", 2);
		free_and_exit(path, cmd, env, 127);
	}
}

static void	analyze_path_of_command(char **path, char ***env, t_command **cmd,
				t_command *current)
{
	if (current->argv[0][0] == '/' || !ft_strncmp(current->argv[0], "./", 2)
		|| !ft_strncmp(current->argv[0], "../", 3))
	{
		*path = ft_strdup(current->argv[0]);
		if (!*path)
			free_and_exit(path, cmd, env, 1);
	}
	else if (find_in_path(*env, current->argv[0], path) == -1)
		free_and_exit(path, cmd, env, 1);
	if (!*path)
	{
		ft_putstr_fd(current->argv[0], 2);
		ft_putendl_fd(": command not found", 2);
		free_and_exit(path, cmd, env, 127);
	}
	check_if_directory(path, cmd, env);
}

void	exec_cmd_child(t_command *current, t_command **cmd, char ***env)
{
	char	*path;
	int		ret;

	path = NULL;
	if (!setup_redirections(current))
		free_and_exit(&path, cmd, env, 1);
	check_for_missing_cmd_and_handle(&path, env, cmd, current);
	if (is_builtin(current->argv[0]))
	{
		ret = execute_builtin(current, 1, env);
		if (ret == -1)
			free_and_exit(&path, cmd, env, 1);
		free_and_exit(&path, cmd, env, ret);
	}
	analyze_path_of_command(&path, env, cmd, current);
	if (execve(path, current->argv, *env) == -1)
		handle_execve_error(current->argv[0], &path, cmd, env);
	free_and_exit(&path, cmd, env, 0);
}
