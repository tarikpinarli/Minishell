/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_single_cmd.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 18:59:42 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/26 14:07:23 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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

	path = NULL;
	if (!cmd->argv || !cmd->argv[0])
		exit(0);
	if (cmd->argv[0][0] == '\0')
		exit(0);
	if (!setup_redirections(cmd, 0))
	{
		free_rest(path, cmd, env);
		exit(1);
	}
	if (cmd->argv[0][0] == '/' || !ft_strncmp(cmd->argv[0], "./", 2) ||
		!ft_strncmp(cmd->argv[0], "../", 3))
		path = ft_strdup(cmd->argv[0]);
	else
		path = find_in_path(env, cmd->argv[0]);
	if (!path)
		command_not_found_err(cmd, path, env);
	check_if_directory(path, cmd, env);
	if (execve(path, cmd->argv, env) == -1)
		handle_execve_error(cmd->argv[0], path, cmd, env);
	exit(1);
}

void	handle_missing_command(t_command *cmd)
{
	if (cmd->in_redir)
		prepare_heredoc_file(cmd, 0);
	else
	{
		ft_putstr_fd("Command ''", 2);
		ft_putendl_fd(" not found, but can be installed", 2);
	}
}

void	exec_command(t_command *cmd, char ***env)
{
	pid_t	pid;
	int		status;

	if ((!cmd || !cmd->argv || !cmd->argv[0] || cmd->argv[0][0] == '\0'))
	{
		handle_missing_command(cmd);
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
