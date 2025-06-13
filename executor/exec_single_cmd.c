/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_single_cmd.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 18:59:42 by tpinarli          #+#    #+#             */
/*   Updated: 2025/06/13 16:41:27 by ykadosh          ###   ########.fr       */
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
static void	run_child_process(t_command *cmd, char ***env)
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
static int	handle_children_status(int status, t_command *cmd,
				int *loop_control_flag)
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
		*loop_control_flag = BREAK;
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
	cleanup_heredocs(cmd);
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
static int	waitpid_error_check(t_command *cmd, pid_t wpid)
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
static int	run_parent_process(t_command *cmd)
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

	if (check_if_builtin_and_execute(cmd, env))
		return (0);
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
