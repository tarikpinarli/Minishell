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
 * run_child_process - Handles execution flow for the child process after fork.
 *
 * This function is called in the child process created by fork().
 * It first sets up custom signal handling (e.g., for SIGINT and SIGQUIT).
 * If signal setup fails, it frees allocated memory and exits with code 1.
 * Then it proceeds to execute the command via exec_single_cmd_child(),
 * which handles redirections, path resolution, and the actual execve call.
 *
 * Parameters:
 *   cmd - The command structure to be executed.
 *   env - The environment variables used by the child process.
 */
void	run_child_process(t_command *cmd, char ***env)
{
	if (setup_signal_handling(0) == -1)
	{
		free_rest(NULL, &cmd, env);
		perror("sigaction");
		exit (1);
	}
	exec_single_cmd_child(&cmd, env);
}

/*
 * Copies the command path directly from the user input
 * (used for absolute or relative paths).
 * - If memory allocation fails during duplication,
 *   prints an error, cleans up, and exits with code 1.
 * Returns the duplicated path string if successful.
 */
char	*copy_path(char *path, t_command *curr, t_command **cmd, char ***env)
{
	path = ft_strdup(curr->argv[0]);
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
char	*extract_path(char *path, t_command *curr, t_command **cmd, char ***env)
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
 * Frees all allocated resources and exits the process with the given exit code.
 * Used in child processes when an error occurs and cleanup is required.
 */
void	free_and_exit(char *path, t_command **cmd, char ***env, int exit_code)
{
	free_rest(&path, cmd, env);
	exit(exit_code);
}
