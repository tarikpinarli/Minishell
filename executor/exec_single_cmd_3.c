/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_single_cmd_3.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/08 13:40:30 by tpinarli          #+#    #+#             */
/*   Updated: 2025/06/08 14:39:25 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
 * Saves the current standard input and output file descriptors.
 *
 * Uses `dup()` to duplicate `STDIN_FILENO` and `STDOUT_FILENO`, storing the
 * duplicates in `saved_stdin` and `saved_stdout`. This allows restoring the
 * original input/output later, especially useful for executing
 * built-in commands
 * that may temporarily modify these streams via redirections.
 *
 * On failure:
 * - Prints an error message using `perror("dup")`.
 * - Closes any successfully duplicated file descriptors to avoid leaks.
 * - Sets the last exit code to 1 and returns early.
 */
static void	save_curr_std(int *saved_stdin, int *saved_stdout)
{
	*saved_stdin = dup(STDIN_FILENO);
	*saved_stdout = dup(STDOUT_FILENO);
	if (*saved_stdin == -1 || *saved_stdout == -1)
	{
		perror("dup");
		if (*saved_stdin != -1)
			close(*saved_stdin);
		if (*saved_stdout != -1)
			close(*saved_stdout);
		(void)last_exit_code(1, 1);
		return ;
	}
}

/*
 * Attempts to apply redirections for an isolated built-in command.
 *
 * If redirection setup (`setup_redirections`) fails, this function restores
 * the original stdin and stdout using `dup2()` to ensure the shell
 * remains usable.
 *
 * Parameters:
 * - cmd: the command node containing redirection info.
 * - in, out: pointers to the saved original stdin and stdout file descriptors.
 *
 * On failure:
 * - Restores original stdin/stdout.
 * - Prints an error message via `perror("dup2")` if restoration fails.
 * - Closes the saved file descriptors.
 * - Sets last exit code to 1 and returns without executing the built-in.
 */
static int	setup_isolated_builtin_redirs(t_command *cmd, int *in, int *out)
{
	if (!setup_redirections(cmd))
	{
		if (dup2(*in, STDIN_FILENO) == -1
			|| dup2(*out, STDOUT_FILENO) == -1)
		{
			perror("dup2");
			if (*in != -1)
				close(*in);
			if (*out != -1)
				close(*out);
			(void)last_exit_code(1, 1);
			return (0);
		}
		close(*in);
		close(*out);
		(void)last_exit_code(1, 1);
		return (0);
	}
	return (1);
}

/*
 * Handles cleanup and exits when a fatal error occurs during
 * isolated built-in execution.
 *
 * This function is called if memory allocation fails or another
 * critical error is encountered
 * during the execution of a built-in command
 * (which runs in the current shell process).
 *
 * Actions performed:
 * - Prints an error message about memory allocation failure.
 * - Frees environment variables, heredoc resources, and the command structure.
 * - Closes the saved stdin and stdout file descriptors.
 * - Clears readline history to free associated memory.
 * - Exits the process with status code 1.
 */
static void	exit_isolated_builtin(char ***env, t_command *cmd, int in, int out)
{
	write(2, ALLOCATION_FAILURE, sizeof(ALLOCATION_FAILURE) - 1);
	free_two_dimensional_array(env);
	cleanup_heredocs(cmd);
	free_cmd(&cmd);
	close(in);
	close(out);
	rl_clear_history();
	exit(1);
}

/*
 * Executes a built-in command in the current process without forking.
 *
 * This is used when the command is a built-in (e.g., `cd`, `export`)
 * and needs to modify the shell's state (like environment variables),
 * which wouldn't persist if done in a child process.
 *
 * Steps:
 * 1. Saves current stdin and stdout file descriptors.
 * 2. Sets up redirections (if any) for the command.
 * 3. Executes the built-in with `execute_builtin()`.
 * 4. If the built-in fails, exits with cleanup.
 * 5. Restores original stdin and stdout after execution.
 * 6. Updates global exit code based on the command’s return value.
 */
static void	exec_isolated_builtin(t_command *cmd, char ***env)
{
	int	saved_stdin;
	int	saved_stdout;
	int	ret;

	save_curr_std(&saved_stdin, &saved_stdout);
	if (!setup_isolated_builtin_redirs(cmd, &saved_stdin, &saved_stdout))
		return ;
	ret = execute_builtin(cmd, 1, env);
	if (ret == -1)
		exit_isolated_builtin(env, cmd, saved_stdin, saved_stdout);
	if (dup2(saved_stdin, STDIN_FILENO) == -1
		|| dup2(saved_stdout, STDOUT_FILENO) == -1)
	{
		perror("dup2");
		if (saved_stdin != -1)
			close(saved_stdin);
		if (saved_stdout != -1)
			close(saved_stdout);
		(void)last_exit_code(1, 1);
		return ;
	}
	close(saved_stdin);
	close(saved_stdout);
	(void)last_exit_code(1, ret);
}

/*
* returns 1 if the input's argument corresponds to a builtin command, after that
* command is executed and the here-documents are cleaned up.
* If there are no arguments or the argument is not a builtin command, returns 0
*/
int	check_if_builtin_and_execute(t_command *cmd, char ***env)
{
	if (cmd->argv && is_builtin(cmd->argv[0]))
	{
		exec_isolated_builtin(cmd, env);
		cleanup_heredocs(cmd);
		return (1);
	}
	return (0);
}
