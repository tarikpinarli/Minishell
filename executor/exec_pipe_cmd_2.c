/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipe_cmd_2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 20:18:08 by tpinarli          #+#    #+#             */
/*   Updated: 2025/06/02 19:32:41 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	update_prev_fd(t_command *cmd, int *prev_fd, int *pipefd)
{
	if (cmd->next)
	{
		close(pipefd[1]);
		*prev_fd = pipefd[0];
	}
	else
		*prev_fd = -1;
}

static void	handle_signal_interception_from_parent(void)
{
	g_signal_status = 0;
	write(1, "\n", 1);
}

static void	wait_for_all_children_after_failure(pid_t wpid, int status)
{
	perror("waitpid");
	wpid = 1;
	while (wpid > 0)
	{
		wpid = waitpid(-1, &status, 0);
		if (g_signal_status == SIGINT)
			handle_signal_interception_from_parent();
		if (wpid == -1)
		{
			if (errno == ECHILD)
				return ;
			else
			{
				perror("waitpid");
				wpid = 1;
				return ;
			}
		}
	}
}

static void	process_status_of_last_cmd(int status)
{
	if (WIFSIGNALED(status))
	{
		if (WTERMSIG(status) == SIGQUIT)
			write(2, "Quit (core dumped)\n",
				(sizeof("Quit (core dumped)\n") - 1));
		(void)last_exit_code(1, 128 + (WTERMSIG(status)));
		if (g_signal_status == SIGINT)
			handle_signal_interception_from_parent();
	}
	else
		(void)last_exit_code(1, WEXITSTATUS(status));
}

void	wait_for_children(pid_t pid, size_t n_children, t_command *cmd)
{
	pid_t	wpid;
	int		status;

	while (n_children)
	{
		wpid = waitpid(-1, &status, 0);
		if (wpid == -1)
		{
			if (errno != ECHILD)
			{
				(void)last_exit_code(1, 1);
				wait_for_all_children_after_failure(wpid, status);
				cleanup_heredocs(cmd);
				return ;
			}
		}
		if (wpid == pid)
			process_status_of_last_cmd(status);
		if (g_signal_status == SIGINT)
			handle_signal_interception_from_parent();
		n_children--;
	}
	cleanup_heredocs(cmd);
}
