/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:30:32 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/21 19:15:02 by tpinarli         ###   ########.fr       */
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

void	exec_cmd_child_logic(t_command *cmd, char ***env)
{
	char	*path;
	int		ret;

	if (!setup_redirections(cmd, 1))
		exit(1);
	if (is_builtin(cmd->argv[0]))
	{
		ret = execute_builtin(cmd, 0, env);
		exit(ret);
	}
	if (cmd->argv[0][0] == '/')
		path = ft_strdup(cmd->argv[0]);
	else
		path = find_in_path(cmd->argv[0]);
	if (!path)
	{
		ft_putstr_fd(cmd->argv[0], 2);
		ft_putendl_fd(": command not found", 2);
		exit(127);
	}
	if (execve(path, cmd->argv, *env) == -1)
	{
		if (errno == EISDIR)
		{
			ft_putstr_fd(cmd->argv[0], 2);
			ft_putendl_fd(": Is a directory", 2);
			exit(126);
		}
		else if (errno == EACCES)
		{
			ft_putstr_fd(cmd->argv[0], 2);
			ft_putendl_fd(": Permission denied", 2);
			exit(126);
		}
		else if (errno == ENOENT)
		{
			ft_putstr_fd(cmd->argv[0], 2);
			ft_putendl_fd(": command not found", 2);
			exit(127);
		}
		else
		{
			perror("execve");
			exit(1);
		}
	}

	free(path);
	exit(1);
}

void	launch_child_process(t_command *cmd, int prv_fd, int *p_fd, char ***env)
{
	int	pid;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return ;
	}
	if (pid == 0)
	{
		prepare_child(cmd, prv_fd, p_fd);
		exec_cmd_child_logic(cmd, env);
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

void	wait_for_children(void)
{
	int	status;
	int	wpid;

	wpid = 1;
	while (wpid > 0)
		wpid = wait(&status);
	last_exit_code(1, WEXITSTATUS(status));
}

void	execute_pipeline(t_command *cmd, char ***env)
{
	int	pipefd[2];
	int	prev_fd;
	int	*curr_pipefd;

	prev_fd = -1;
	while (cmd)
	{
		if (!prepare_heredoc_file(cmd))
    		return ;
		curr_pipefd = NULL;
		if (cmd->next)
		{
			if (!setup_pipe(pipefd))
				return ;
			curr_pipefd = pipefd;
		}
		launch_child_process(cmd, prev_fd, curr_pipefd, env);
		if (prev_fd != -1)
			close(prev_fd);
		update_prev_fd(cmd, &prev_fd, pipefd);
		cmd = cmd->next;
	}
	wait_for_children();
}

int	exec_isolated_builtin(t_command *cmd, char ***env)
{
	int	saved_stdin;
	int	saved_stdout;
	int	ret;

	saved_stdin = dup(STDIN_FILENO);
	saved_stdout = dup(STDOUT_FILENO);
	if (!setup_redirections(cmd, 0))
	{
		dup2(saved_stdin, STDIN_FILENO);
		dup2(saved_stdout, STDOUT_FILENO);
		close(saved_stdin);
		close(saved_stdout);
		last_exit_code(1, 1);
		return (1);
	}
	ret = execute_builtin(cmd, 1, env);
	dup2(saved_stdin, STDIN_FILENO);
	dup2(saved_stdout, STDOUT_FILENO);
	close(saved_stdin);
	close(saved_stdout);
	last_exit_code(1, ret);
	cleanup_heredocs(cmd);
	return (1);
}

void	exec_single_cmd_child(t_command *cmd, char **env)
{
	char	*path;

	if (!cmd->argv || !cmd->argv[0])
		exit(0);
	if (cmd->argv[0][0] == '\0')
		exit(0);

	if (!setup_redirections(cmd, 0))
		exit(1);
	if (cmd->argv[0][0] == '/' ||
		ft_strncmp(cmd->argv[0], "./", 2) == 0 ||
		ft_strncmp(cmd->argv[0], "../", 3) == 0)
		path = ft_strdup(cmd->argv[0]);
	else
		path = find_in_path(cmd->argv[0]);
	if (!path)
	{
		write(2, cmd->argv[0], ft_strlen(cmd->argv[0]));
		write(2, ": command not found\n", 21);
		exit(127);
	}
	if (execve(path, cmd->argv, env) == -1)
	{
		if (errno == EISDIR)
		{
			ft_putstr_fd(cmd->argv[0], 2);
			ft_putendl_fd(": Permission denied", 2);
			exit(126);
		}
		else if (errno == EACCES)
		{
			ft_putstr_fd(cmd->argv[0], 2);
			ft_putendl_fd(": Is a directory", 2);
			exit(126);
		}
		else if (errno == ENOENT)
		{
			ft_putstr_fd(cmd->argv[0], 2);
			ft_putendl_fd(": command not found", 2);
			exit(127);
		}
		else
		{
			perror("execve");
			exit(1);
		}
	}
	free(path);
	exit(1);
}

void	exec_command(t_command *cmd, char ***env)
{
	pid_t	pid;
	int		status;

	if (!cmd || !cmd->argv || !cmd->argv[0] || cmd->argv[0][0] == '\0')
	{
		ft_putstr_fd("Command ''", 2);
		ft_putendl_fd(" not found, but can be installed", 2);
		return ;
	}
	if (is_builtin(cmd->argv[0]) && !cmd->next)
		if (exec_isolated_builtin(cmd, env) == 1)
			return ;
	pid = fork();
	if (pid == -1)
	{
		perror("fork failed");
		return ;
	}
	if (pid == 0)
		exec_single_cmd_child(cmd, (*env));
	else
	{
		waitpid(pid, &status, 0);
		last_exit_code(1, WEXITSTATUS(status));
	}
}
