/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipe_cmd_1.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:30:32 by tpinarli          #+#    #+#             */
/*   Updated: 2025/06/01 11:49:16 by ykadosh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	exec_cmd_child_logic(t_command *cmd, char ***env)
{
	char	*path;
	int		ret;

	if (!setup_redirections(cmd))   // TODO: we need to free the child process' heap memory if we exit
		exit(1);
	if (is_builtin(cmd->argv[0]))
	{
		ret = execute_builtin(cmd, 0, env);
		exit(ret);
	}
	if (cmd->argv[0][0] == '/' || !ft_strncmp(cmd->argv[0], "./", 2)
		|| !ft_strncmp(cmd->argv[0], "../", 3))
		path = ft_strdup(cmd->argv[0]); // TODO: malloc() failure protection
	else
		path = find_in_path(*env, cmd->argv[0]);
	if (!path)
	{
		ft_putstr_fd(cmd->argv[0], 2);
		ft_putendl_fd(": command not found", 2);
		exit(127);
	}
	check_if_directory(path, cmd, *env);
	if (execve(path, cmd->argv, *env) == -1)
		handle_execve_error(cmd->argv[0], path, cmd, *env);
	free(path);
	exit(1);
}

// TODO: Error handling! this is just a DRAFT!!!
int	launch_child_process(t_command *cmd, int prev_fd, int *p_fd, char ***env, pid_t *pid)
{
	*pid = fork();
	if (*pid == -1)
	{
		perror("fork");
		return (1);
	}
	if (*pid == 0)
	{
		if (setup_signal_handling(0) == -1) // WARN: recently added - but the parent processing the results hasn't been done yet.
		{
			free_rest(NULL, &cmd, env); // WARN: I am not sure at all anymore
			//	regarding freeing the allocated memory in the child!
			return (3); // this return value tells the parent to call perror("sigaction");
		}
		prepare_child(cmd, prev_fd, p_fd); // TODO: handle the malloc() failures.
		exec_cmd_child_logic(cmd, env); // TODO : handle the malloc() failures (and others?)
	}
	return (0);
}

/*
* return values:
* 1: open() failure or SIGINT intercepted during heredoc
* 2: command not found (or empty argument/s provided with heredocs)
* 0: if execution went smoothly
*/
int	execute_pipeline(t_command *cmd, char ***env)
{
	int			pipefd[2];
	int			prev_fd;
	int			*curr_pipefd;
	int			failure_flag;
	pid_t		pid;
	t_command	*current;

	prev_fd = -1;
	failure_flag = 0;
	current = cmd;
	// NOTE: It would be great to refactor this next while loop into a separate function, which would be called before
	// exec_cmd() and before execute_pipeline - I believe it is the same for both!
	while (current) // 1st loop: goes throught the whole command to open all heredocs (even ones in different pipes!)
	{
		failure_flag = prepare_heredoc_file(current);
		if (failure_flag)
		{
			if (failure_flag == -2) // malloc() failed
			{
				cleanup_heredocs(cmd); // WARN: needs check for whether this is necessary...
				free_rest(NULL, &cmd, env); // WARN: is there at some point "path" being allocated and existing here?
				write(2, ALLOCATION_FAILURE, sizeof(ALLOCATION_FAILURE) - 1);
				exit (last_exit_code(1, 1));
			}
			else // open() failed OR sigint was intercepted in the heredoc; env() should not be freed - unless we are in the child process!
				return (1); // if you need a return value: 1
		}
		current = current->next;
	}
	current = cmd;
	while (current)
	{
		/*
		failure_flag = prepare_heredoc_file(current);
		if (failure_flag)
		{
			if (failure_flag == -2) // malloc() failed
			{
				cleanup_heredocs(current); // WARN: needs check for whether this is necessary...
				free_rest(NULL, &current, env); // WARN: is there at some point "path" being allocated and existing here?
				write(2, ALLOCATION_FAILURE, sizeof(ALLOCATION_FAILURE) - 1);
				exit (last_exit_code(1, 1));
			}
			else // open() failed OR sigint was intercepted in the heredoc; env() should not be freed - unless we are in the child process!
				return (1); // if you need a return value: 1
		}
		*/

		// TODO: this section needs to be reviewed.
		// TODO: put here the REDIRECTIONS, and only execute commands afterwards!
		if (!current->argv) // makes sure not to have a segfault later on if we have no arguments in the current cmd list.
			return (0);
		if (current->argv[0] && !current->argv[0][0]) // this means the first command is an empty string
		{
			ft_putendl_fd("Command '' not found", 2);
			(void)last_exit_code(1, 127);
			return (2);
		}
		curr_pipefd = NULL;
		if (current->next)
		{
			if (!setup_pipe(pipefd))
			{
				// TODO: how about all the other processes?
				return (1); // if you need a return value: 1
			}
			curr_pipefd = pipefd;
		}
		// TODO: try to catch errors with failure_flag?
		failure_flag = launch_child_process(current, prev_fd, curr_pipefd, env, &pid);
		if (failure_flag)
		{
			if (failure_flag == 1) // fork failed. no child process created. But there could be other children already open...
				return (1);
			// else // TODO: handle errors from lanch child process

		}
		if (prev_fd != -1)
			close(prev_fd);
		update_prev_fd(current, &prev_fd, pipefd);
		current = current->next;
	}
	wait_for_children(pid);
	return (0);
}
