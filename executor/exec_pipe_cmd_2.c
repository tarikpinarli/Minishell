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

int	setup_pipe(int *pipefd)
{
	if (pipe(pipefd) == -1)
	{
		perror("pipe");
		return (0);
	}
	return (1);
}

void	prepare_child(t_command *current, int prev_fd, int *pipefd)
{
	if (prev_fd != -1)
	{
		dup2(prev_fd, STDIN_FILENO);
		close(prev_fd);
	}
	if (current->next)
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

/*
 * NOTE: this is another work around for cases such as "cat | echo hi" and/or "cat | cat | cat" and SIGINT passed
 * I keep this here if ever the current solution does not work at some point...
 * The check for wether the global variable is SIGINT which prints a newline and resets it to zero
 * is the last chosen solution.
static void	handle_sigint_while_waiting(int sig)
{
	write(1, "\n", 1);
	(void)sig;
}
*/

int	wait_for_children(pid_t pid, size_t n_of_children)
{
	pid_t	wpid;
	int		status;

	/*
	 * NOTE: this is part of the solution that uses sigaction to ignore SIGINT when waiting to the child.
	 * Remove when ready, for now I keep those in case something goes wrong during testing...
	struct sigaction	sa_int;

	ft_bzero(&sa_int, sizeof(sigaction));
	(void)sigemptyset(&sa_int.sa_mask);
	(void)sigaddset(&sa_int.sa_mask, SIGINT);
	sa_int.sa_flags = SA_RESTART;
	sa_int.sa_handler = &handle_sigint_while_waiting;
	if (sigaction(SIGINT, &sa_int, NULL) == -1)
	{
		perror("sigaction");
		return (-1);  // WARN: follow through with this at the caller....
	}
	*/
	while (n_of_children)
	{
		wpid = waitpid(-1, &status, 0);
		if (wpid == -1)
		{
			perror("waitpid");
			return (1);
		}
		if (wpid == pid)
		{
			if (WIFSIGNALED(status))
			{
				if (WTERMSIG(status) == SIGQUIT)
					write(2, "Quit (core dumped)\n",
						(sizeof("Quit (core dumped)\n") - 1));
				last_exit_code(1, 128 + (WTERMSIG(status)));
				if (g_signal_status == SIGINT) // necessary to put this block here too, as well as for all subprocesses, in the case of "echo hi | sleep 10" (or the other way around)
				{
					g_signal_status = 0;
					write(1, "\n", 1);
				}
			}
			else
				last_exit_code(1, WEXITSTATUS(status));
		}
		if (g_signal_status == SIGINT)
		{
			g_signal_status = 0;
			write(1, "\n", 1);
		}
		n_of_children--;
	}
	/*
	 * NOTE: this is part of the solution that uses the struct declared at the start of this function. Remove when ready, for now I keep those in case something goes wrong during testing...
	sa_int.sa_handler = &handle_sigint;
	sigaction(SIGINT, &sa_int, NULL);
	*/
	return (0);
}
