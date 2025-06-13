/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipe_cmd.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:30:32 by tpinarli          #+#    #+#             */
/*   Updated: 2025/06/13 16:38:11 by ykadosh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	prepare_child(t_command *current, int prev_fd, int *pipefd)
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

static int	launch_child_process(t_pipeline *pipeline, t_command *current,
		t_command **cmd, char ***env)
{
	pipeline->pid = fork();
	if (pipeline->pid == -1)
	{
		perror("fork");
		(void)last_exit_code(1, 1);
		return (0);
	}
	if (pipeline->pid == 0)
	{
		if (setup_signal_handling(0) == -1)
		{
			perror("sigaction");
			free_rest(NULL, cmd, env);
			exit (1);
		}
		prepare_child(current, pipeline->prev_fd, pipeline->curr_pipefd);
		exec_cmd_child(current, cmd, env);
	}
	return (1);
}

static int	setup_pipe(int *pipefd, t_command *cmd)
{
	if (pipe(pipefd) == -1)
	{
		perror("pipe");
		(void)last_exit_code(1, 1);
		return (0);
	}
	return (1);
}

static void	init_pipeline(t_pipeline *pipeline)
{
	ft_bzero(pipeline, sizeof(t_pipeline));
	pipeline->prev_fd = -1;
}

void	execute_pipeline(t_command *cmd, char ***env)
{
	t_pipeline	pipeline;
	t_command	*current;

	init_pipeline(&pipeline);
	current = cmd;
	while (current)
	{
		if (current->next)
		{
			if (!setup_pipe(pipeline.pipefd, cmd))
				break ;
			pipeline.curr_pipefd = pipeline.pipefd;
		}
		if (!launch_child_process(&pipeline, current, &cmd, env))
			break ;
		if (pipeline.pid > 0)
			pipeline.n_children++;
		if (pipeline.prev_fd != -1)
			close(pipeline.prev_fd);
		update_prev_fd(current, &pipeline.prev_fd, pipeline.pipefd);
		current = current->next;
	}
	wait_for_children(pipeline.pid, pipeline.n_children, cmd);
}
