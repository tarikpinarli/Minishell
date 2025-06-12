/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_single_cmd_2.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/08 13:40:07 by tpinarli          #+#    #+#             */
/*   Updated: 2025/06/08 14:26:37 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
 * Copies the command path directly from the user input
 * (used for absolute or relative paths).
 * - If memory allocation fails during duplication,
 *   prints an error, cleans up, and exits with code 1.
 * Returns the duplicated path string if successful.
 */
static char	*copy_path(char *path, t_command *cur, t_command **cmd, char ***env)
{
	path = ft_strdup(cur->argv[0]);
	if (!path)
	{
		write(2, "memory allocation failure in child process\n",
			sizeof("memory allocation failure in child process\n") - 1);
		free_rest(&path, cmd, env);
		exit(1);
	}
	return (path);
}

/*
 * Attempts to locate the executable for a command using the PATH
 * environment variable.
 * - If memory allocation fails, prints an error and exits with code 1.
 * - If the command is not found in any PATH directory, prints an error
 *   and exits with code 127.
 * Returns the full path to the executable if successful.
 */
static char	*extract_path(char *path, t_command *curr, t_command **cmd,
				char ***env)
{
	if (find_in_path(*env, curr->argv[0], &path) == -1)
	{
		write(2, "memory allocation failure in child process\n",
			sizeof("memory allocation failure in child process\n") - 1);
		free_rest(&path, cmd, env);
		exit (1);
	}
	if (!path)
	{
		ft_putstr_fd(curr->argv[0], 2);
		ft_putendl_fd(": command not found", 2);
		free_rest(&path, cmd, env);
		exit(127);
	}
	return (path);
}

/*
 * Executes a single external command in the child process.
 *
 * Steps:
 * - Applies redirections. If it fails, exits with code 1.
 * - If the command is empty, exits with code 0.
 * - Resolves the path of the command:
 *     - If it's an absolute or relative path (e.g., "./a.out"), use it directly.
 *     - Otherwise, search for it in the system's PATH.
 * - Checks if the path points to a directory (which is not executable).
 * - Calls execve() to run the command. If it fails, handles the error.
 * - Cleans up and exits if execve() does not succeed.
 *
 * This function never returns; it always ends with exit().
 */
void	exec_single_cmd_child(t_command **cmd, char ***env)
{
	char		*path;
	t_command	*current;

	path = NULL;
	current = *cmd;
	if (!setup_redirections(current))
		free_and_exit(&path, cmd, env, 1);
	if (!current->argv || !current->argv[0] || !current->argv[0][0])
		free_and_exit(&path, cmd, env, 0);
	if (current->argv[0][0] == '/' || !ft_strncmp(current->argv[0], "./", 2)
		|| !ft_strncmp(current->argv[0], "../", 3))
		path = copy_path(path, current, cmd, env);
	else
		path = extract_path(path, current, cmd, env);
	check_if_directory(&path, cmd, env);
	if (execve(path, current->argv, *env) == -1)
		handle_execve_error(current->argv[0], &path, cmd, env);
	free_rest(&path, cmd, env);
	exit(0);
}
