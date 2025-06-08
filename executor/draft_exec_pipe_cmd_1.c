/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draft_exec_pipe_cmd_1.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:30:32 by tpinarli          #+#    #+#             */
/*   Updated: 2025/06/08 18:18:35 by ykadosh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// TESTING: Try stuff like :	1. echo hello | '' > file1
//								2. echo hello | '' > file2 | echo hi
//								3. echo hello | cat Makefile | "" > file4
//						also:	4. > file1 ''
//								5. > file2 '' echo


void	exec_cmd_child_logic(t_command *current, t_command **cmd, char ***env)
{
	char	*path;
	int		ret;

	path = NULL;
	if (!setup_redirections(current))   // TODO: we need to free the child process' heap memory if we exit
		exit(1);
	if (is_builtin(current->argv[0]))
	{
		ret = execute_builtin(current, 1, env);
		if (ret == -1)
		{
			free_rest(&path, cmd, env);
			exit (12);
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
			exit(12);
		}
	}
	else if (find_in_path(*env, current->argv[0], &path) == -1)
	{
		free_rest(&path, cmd, env);
		exit(12);
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
		handle_execve_error(current->argv[0], path, cmd, env);
	free_rest(&path, cmd, env);
	exit(1);
}

int	launch_child_process(t_command *current, t_command **cmd, int prev_fd, int *p_fd, char ***env, pid_t *pid)
{
	*pid = fork();
	if (*pid == -1) // fork() failed
	{
		perror("fork");
		return (1);
	}
	if (*pid == 0)
	{
		if (setup_signal_handling(0) == -1) // sigaction() failed
		{
			perror("sigaction");
			cleanup_heredocs(cmd);
			free_rest(NULL, cmd, env);
			// NOTE: close_fds() here ????
			exit (1);
		}
		// TODO: we are hereeeee: I don't feel like I took care of the rest of this current function from this point onwards
		prepare_child(current, prev_fd, p_fd); // TODO: handle the malloc() failures.
		exec_cmd_child_logic(current, cmd, env); // TODO : handle the malloc() failures (and others?)
		// NOTE: close fds? cleaup_heredocs() ? exit() ?? free_rest()?? to be done
	}
	exit (0);
}

/*
 * WARN: when this function is ready, review the return values
* return values:
* 1: open() or fork() failure, OR SIGINT intercepted during heredoc
* 2: command not found (or empty argument/s provided with heredocs)
* 0: if execution went smoothly
*/
int	execute_pipeline(t_command *cmd, char ***env)
{
	int			pipefd[2];
	int			*curr_pipefd;
	int			prev_fd;
	t_command	*current;
	pid_t		pid;
	pid_t		wpid;
	int			status;
	int			failure_flag;

	prev_fd = -1;
	failure_flag = 0;
	pid = 0;
	wpid = 0;
	current = cmd;
	while (current)
	{
		curr_pipefd = NULL;
		if (current->next)
		{
			if (!setup_pipe(pipefd))
			{
				cleanup_heredocs(cmd);
				return (1); // if you need a return value: 1
			}
			curr_pipefd = pipefd;
		}
		failure_flag = launch_child_process(current, &cmd, prev_fd, curr_pipefd, env, &pid);
		// if (failure_flag) // TODO: are there still other failure flags in launch_child_process() that are not handled for now?
		// {
		if (failure_flag == 1 || WEXITSTATUS(status) == 1) // fork failed || sigaction() failed -- TODO: let's redo this part, and only have a waitpid() at the very end, first for the specific rightmost child, followed by a waitpid for -1 (for all children, until wpid returns -1 && errno is set to ECHILD.
		{
			wpid = waitpid(-1, &status, 0); // we need to stop the loop, clean and return the prompt; but first, wait for ALL children to finish!
			if (wpid == -1) // happens if no more subprocesses are on OR if some other error of watipid() occured...
			{
				if (errno != ECHILD) // waitpid failed, but NOT because there are no more children to wait for.
				{
					perror("waitpid");
					errno = 0;
					while (errno != ECHILD) // we might still need to wait for other children...
						wpid = waitpid(-1, &status, 0);
					cleanup_heredocs(cmd);
					return (2); // waitpid() failed after fork() failed
				}
				cleanup_heredocs(cmd);
				return (3);  // fork failed
			}
		}
		// else // TODO: handle errors from launch child process


		if (prev_fd != -1) // Question to Tarik: should this be before or after the check for an empty argument?
			close(prev_fd);
		update_prev_fd(current, &prev_fd, pipefd);


		if (current->argv && current->argv[0] && !current->argv[0][0]) // this means the first command is an empty string. WARN: the issue here is that the redirections are not happening because this is here. We have to put the redirections before.
		{
			ft_putendl_fd("Command '' not found", 2);
			current = current->next;
			if (!current)
			{
				(void)last_exit_code(1, 127);
				break ;
			}
		}
		current = current->next;
	}
	// TODO: make sure that you still wait for all children, until wpid becmoes -1 && errno == ECHILD, because the first child might return before the others?
	if (wpid == pid) // process return / exit status from the rightmost child process; update last exit status accordingly; cleanup_heredocs(), error handling
		wait_for_children(pid, n_of_children);

	return (0);
}
