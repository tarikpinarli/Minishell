/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_single_cmd.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 18:59:42 by tpinarli          #+#    #+#             */
/*   Updated: 2025/06/08 14:16:21 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	setup_isolated_builtin_redirections(t_command *cmd, int *in, int *out)
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
			return ;
		}
		close(*in);
		close(*out);
		(void)last_exit_code(1, 1);
		return ;
	}
}

void	save_curr_std(int *saved_stdin, int *saved_stdout)
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

void	exit_isolated_builtin(char ***env, t_command *cmd, int in, int out)
{
	write(2, ALLOCATION_FAILURE, sizeof(ALLOCATION_FAILURE) -1);
	free_two_dimensional_array(env);
	cleanup_heredocs(cmd);
	free_cmd(&cmd);
	close(in);
	close(out);
	rl_clear_history();
	exit(1);
}

void	exec_isolated_builtin(t_command *cmd, char ***env)
{
	int	saved_stdin;
	int	saved_stdout;
	int	ret;

	save_curr_std(&saved_stdin, &saved_stdout);
	setup_isolated_builtin_redirections(cmd, &saved_stdin, &saved_stdout);
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

void	free_and_exit(char *path, t_command **cmd, char ***env, int exit_code)
{
	free_rest(&path, cmd, env);
	exit(exit_code);
}

char	*copy_path(char *path, t_command *curr, t_command **cmd, char ***env)
{
	path = ft_strdup(curr->argv[0]);
	if (!path) // malloc failure
	{
		write(2, "memory allocation failure in child process\n",
			sizeof("memory allocation failure in child process\n") - 1);
		free_rest(&path, cmd, env);
		exit(1);
	}
	return (path);
}

char	*extract_path(char *path, t_command *curr, t_command **cmd, char ***env)
{
	if (find_in_path(*env, curr->argv[0], &path) == -1) // malloc failure
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
		free_and_exit(path, cmd, env, 1);
	if (!current->argv || !current->argv[0] || !current->argv[0][0])
		free_and_exit(path, cmd, env, 0);
	if (current->argv[0][0] == '/' || !ft_strncmp(current->argv[0], "./", 2)
		|| !ft_strncmp(current->argv[0], "../", 3))
		path = copy_path(path, current, cmd, env);
	else
		path = extract_path(path, current, cmd, env);
	check_if_directory(&path, cmd, env);
	if (execve(path, current->argv, *env) == -1)
		handle_execve_error(current->argv[0], path, cmd, env);
	free_rest(&path, cmd, env);
	exit(0);
}

int	prepare_builtin(t_command *cmd, char ***env)
{
	if (cmd->argv && is_builtin(cmd->argv[0]))
	{
		exec_isolated_builtin(cmd, env);
		cleanup_heredocs(cmd);
		return (0);
	}
	return (-1);
}

/*
 * Handles the exit status of a child process.
 * Checks if the child was stopped by a signal or exited normally.
 *
 * If the child was killed by:
 * - SIGQUIT → prints "Quit (core dumped)"
 * - SIGINT  → prints a newline
 * Then sets the exit code to 128 + signal number.
 *
 * If the child exited normally with code 1:
 * - Cleans up heredocs and sets exit code to 1.
 *
 * In all cases, tells the parent to stop waiting for more children.
 *
 * Returns:
 * 1 if the child exited with status 1
 * -1 in all other cases (signal or other exit code)
 */
int	handle_children_status(int status, t_command *cmd, int *loop_control_flag)
{
	if (WIFSIGNALED(status))
	{
		if (WTERMSIG(status) == SIGQUIT)
			write(2, "Quit (core dumped)\n",
				(sizeof("Quit (core dumped)\n") - 1));
		else if (WTERMSIG(status) == SIGINT)
			write(1, "\n", 1);
		(void)last_exit_code(1, 128 + (WTERMSIG(status)));
		g_signal_status = 0;
		*loop_control_flag = BREAK; // WARN: we probably want to return from here, clean up everything (heredocs, cmd) and return the loop, otherwise it will still output something like command not found, which has been an issue....
	}
	else
	{
		if (WEXITSTATUS(status) == 1)
		{
			cleanup_heredocs(cmd);
			(void)last_exit_code(1, 1);
			return (1);
		}
		(void)last_exit_code(1, WEXITSTATUS(status));
		*loop_control_flag = BREAK;
	}
	return (-1);
}

/*
 * Checks for errors returned by waitpid().
 * If waitpid fails due to an actual error (excluding ECHILD),
 * it prints an error message, cleans up heredocs, and returns 1.
 *
 * Note:
 * - ECHILD means there are no remaining child processes to wait for,
 *   which is not treated as a real error in this context.
 *
 * Returns:
 * 1 if waitpid failed due to a real error
 * -1 if no actionable error occurred (e.g., no more child processes)
 */
int	waitpid_error_check(t_command *cmd, pid_t wpid)
{
	if (wpid == -1)
	{
		if (errno != ECHILD)
		{
			perror("waitpid");
			cleanup_heredocs(cmd);
			return (1);
		}
	}
	return (-1);
}

/*
 * Waits for child processes to terminate and handles their exit status.
 * Detects signals like SIGINT or SIGQUIT and updates the global exit code.
 * Also checks for empty commands (e.g., "") after all children have exited.
 *
 * Returns:
 * 1 if a child exited with error or waitpid failed
 * 2 if the command was an empty string (e.g., "")
 * -1 if all children exited normally and no specific condition was met
 */
int	run_parent_process(t_command *cmd)
{
	pid_t	wpid;
	int		status;
	int		loop_control_flag;
	int		ret;

	while (1)
	{
		wpid = waitpid(-1, &status, 0);
		ret = waitpid_error_check(cmd, wpid);
		if (ret != -1)
			return (ret);
		ret = handle_children_status(status, cmd, &loop_control_flag);
		if (loop_control_flag == BREAK)
			break ;
		if (ret != -1)
			return (ret);
	}
	if (cmd->argv && cmd->argv[0] && !cmd->argv[0][0])
	{
		ft_putendl_fd("Command '' not found", 2);
		(void)last_exit_code(1, 127);
		cleanup_heredocs(cmd);
		return (2);
	}
	return (-1);
}

/*
 * Executes a single command, either as a builtin or an external program.
 * Handles heredoc setup, forks a child process for execution,
 * and waits in the parent for its completion.
 *
 * Returns:
 * 0 if the command was executed successfully
 * 1 if a system error occurred (e.g., fork, heredoc, or exec failure)
 * 2 if the command was not found (e.g., empty input or missing binary)
 */
int	exec_single_command(t_command *cmd, char ***env)
{
	pid_t	pid;
	int		ret;

	ret = prepare_builtin(cmd, env);
	if (ret != -1)
		return (ret);
	pid = fork();
	if (pid == -1)
	{
		perror("fork failed");
		cleanup_heredocs(cmd);
		(void)last_exit_code(1, 1);
		return (1);
	}
	if (pid == 0)
		run_child_process(cmd, env);
	else
	{
		ret = run_parent_process(cmd);
		if (ret != -1)
			return (ret);
	}
	cleanup_heredocs(cmd);
	return (0);
}
