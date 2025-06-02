/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipe_cmd_2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 20:18:08 by tpinarli          #+#    #+#             */
/*   Updated: 2025/06/01 12:13:18 by ykadosh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	setup_pipe(int *pipefd)
{
	if (pipe(pipefd) == -1)
	{
		perror("pipe");
		return (0);
	}
	return (1);
}

void	prepare_child(t_command *cmd, int prev_fd, int *pipefd)
{
	if (prev_fd != -1)
	{
		dup2(prev_fd, STDIN_FILENO);
		close(prev_fd);
	}
	if (cmd->next)
	{
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);
	}
}

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

int	wait_for_children(pid_t pid)
{
	pid_t	wpid;
	int		status;

	while (1)
	{
		wpid = waitpid(-1, &status, 0);
		if (wpid == -1)
		{
			perror("waitpid");
			return (1);
		}
		if (WIFSIGNALED(status))
		{
			if (WTERMSIG(status) == SIGQUIT)
				write(2, "Quit (core dumped)\n",
					(sizeof("Quit (core dumped)\n") - 1));
			else if (WTERMSIG(status) == SIGINT)
				write(1, "\n", 1);
			last_exit_code(1, 128 + (WTERMSIG(status)));
			g_signal_status = 0;
		}
		else
		{
			if (WEXITSTATUS(status) == 3)
			{
				perror("sigaction");
				last_exit_code(1, 1);
				return (1);
			}
			if (wpid == pid)
			{
				last_exit_code(1, WEXITSTATUS(status));
				printf("ALL CHILDREN EXITED CLEANLY\n\n"); // WARN: just debugging line here
				break ;
			}
		}
	}
	return (0);
}
