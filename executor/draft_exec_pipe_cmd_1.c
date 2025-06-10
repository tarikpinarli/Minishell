/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draft_exec_pipe_cmd_1.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:30:32 by tpinarli          #+#    #+#             */
/*   Updated: 2025/06/10 11:44:26 by ykadosh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// TODO: refactor me this!
void	exec_cmd_child_logic(t_command *current, t_command **cmd, char ***env)
{
	char	*path;
	int		ret;

	path = NULL;
	if (!setup_redirections(current))
	{
		free_rest(&path, cmd, env);
		exit(1);
	}
	if (!current->argv)
	{
		free_rest(&path, cmd, env);
		exit (0);
	}
	// WARN: If you delete from above this control structure the check for "if (!current->argv)", put it back at the start of the next if statement!!!
	if (current->argv[0] && !current->argv[0][0])
	{
		ft_putendl_fd("Command '' not found", 2);
		free_rest(&path, cmd, env);
		exit (127);
	}
	if (is_builtin(current->argv[0]))
	{
		ret = execute_builtin(current, 1, env);
		if (ret == -1)
		{
			free_rest(&path, cmd, env);
			exit (1);
		}
		free_rest(&path, cmd, env);
		exit (ret);
	}
	if (current->argv[0][0] == '/' || !ft_strncmp(current->argv[0], "./", 2)
		|| !ft_strncmp(current->argv[0], "../", 3))
	{
		path = ft_strdup(current->argv[0]);
		if (!path)
		{
			free_rest(&path, cmd, env);
			exit(1); // malloc() failure exits: always 1
		}
	}
	else if (find_in_path(*env, current->argv[0], &path) == -1)
	{
		free_rest(&path, cmd, env);
		exit(1); // malloc() failure exits: always 1
	}
	if (!path)
	{
		ft_putstr_fd(current->argv[0], 2);
		ft_putendl_fd(": command not found", 2);
		free_rest(&path, cmd, env);
		exit(127);
	}
	check_if_directory(&path, cmd, env);
	if (execve(path, current->argv, *env) == -1)
		handle_execve_error(current->argv[0], &path, cmd, env);
	free_rest(path, cmd, env);
	exit(0);
}

static int	launch_child_process(t_pipeline *pipeline, t_command *current,
		t_command **cmd, char ***env)
{
	pipeline->pid = fork();
	if (pipeline->pid == -1) // fork() failed
	{
		perror("fork");
		cleanup_heredocs(*cmd);
		return (-1);
	}
	if (pipeline->pid == 0)
	{
		if (setup_signal_handling(0) == -1) // sigaction() failed
		{
			perror("sigaction");
			free_rest(NULL, cmd, env);
			exit (1);
		}
		prepare_child(current, pipeline->prev_fd, pipeline->curr_pipefd);
		exec_cmd_child_logic(current, cmd, env);
	}
	return (0);
}

static void	init_pipeline(t_pipeline *pipeline)
{
	ft_bzero(pipeline, sizeof(t_pipeline));
	pipeline->prev_fd = -1;
}

/*
 * WARN: when this function is ready, review the return values
* return values:
* 1: open() or fork() failure, OR SIGINT intercepted during heredoc
* 2: command not found (or empty argument/s provided with heredocs)
* -1: upon failure of waitpid(); In this case, Minishell still waits for all
* children that might still be active and marks the failure with perror().
* 0: if execution went smoothly
*/
int	execute_pipeline(t_command *cmd, char ***env)
{
	t_pipeline	pipeline;
	t_command	*current;

	init_pipeline(&pipeline);
	current = cmd;
	while (current)
	{
		if (current->next)
		{
			if (!setup_pipe(pipeline.pipefd, cmd)) // pipe() failed
				break ;
			pipeline.curr_pipefd = pipeline.pipefd;
		}
		if (launch_child_process(&pipeline, current, &cmd, env) == -1) // fork() failed, we do not continue executing more pipes, but we wait for all processes
			break ;
		if (pipeline.pid > 0)
			pipeline.n_children++;
		if (pipeline.prev_fd != -1)
			close(pipeline.prev_fd);
		update_prev_fd(current, &pipeline.prev_fd, pipeline.pipefd);
		current = current->next;
	}
	return (wait_for_children(pipeline.pid, pipeline.n_children, cmd));
}
