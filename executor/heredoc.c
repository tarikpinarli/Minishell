/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykadosh <ykadosh@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 14:20:46 by ykadosh           #+#    #+#             */
/*   Updated: 2025/06/06 14:33:28 by ykadosh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static
int	heredoc_readline_loop(char *delimiter, char *file_name, int fd)
{



}


/*
* return values:
* -1 upon failure of open()
* -2 upon malloc() failure
* -3 upon interception of SIGINT during heredoc's readline
* 0, otherwise
*/
static
int	open_heredoc_files(t_redir *in_redir, char *delimiter, int i, char ***env)
{
	char		*file_number;
	char		*file_name;
	char		*line;
	int			fd;

// WARN: add O_EXCL to open()'s flags, and check against Errno's EEXIST, and
// include this whole block in a loop incrementing i until a non pre-existing
// file is created: this avoids a pre-existing file (called "heredoc_x")
// from having its contains deleted and overwritten by Minishell's temporary
// heredoc file creation....
// CAREFUL however: this should be done in concert with cleanup_heredocs()...
// The best would be to unlink everything already here and to cleanup
// if something fails here... That should be done in a way that conserves the
// file descriptors opened and passes them around to the execution of commands -
// the unlinking only removes the name of the file, making it invisible, but as
// long as the program has at least one fd to that file, it remains "alive"
// and accessible to the program.

	while (1)
	{
		file_number = ft_itoa(i);
		if (!file_number)
			return (-2);
		file_name = ft_strjoin("heredoc_", file_number);
		free(file_number);
		if (!file_name)
			return (-2);
		fd = open(file_name, O_WRONLY | O_CREAT | O_EXCL | S_IRUSR | S_IWUSR); // 0600 permissions, for heredoc, only the process should write to those temps
		if (fd < 0)
		{
			if (errno == EEXIST)
			{
				i++;
				free(file_name);
				continue;
			}
			else
			{
				perror(file_name);
				free(file_name);
				return (-1);
			}
		}
		break ;
	}
	// TODO: heredoc_readline_loop()
	while (1)
	{
		rl_event_hook = &heredoc_signal_hook;
		line = readline("\001\033[1m\002heredoc> \001\033[0m\002");
		if (!line || !ft_strcmp(line, delimiter) || g_signal_status == SIGINT)
			break ;
		if (!in_redir->is_heredoc_delimiter_quoted)
		{
			if (check_if_str_contains_vars_to_expand(line))
				if (!rebuild_expandable_heredoc_line(&line, env)) // WARN: malloc() unprotected?
					return (-2);
		}
		if (line) // line might be NULL after rebuild_expandable_heredoc_line() (if a var expanded to nothing), and if we don't check for it, ft_strlen will segfault!
			write(fd, line, ft_strlen(line));
		write(fd, "\n", 1);
		free(line);
	}
	free(line);
	close(fd);
	if (g_signal_status == SIGINT)
	{
		free(in_redir->filename);
		in_redir->filename = file_name;
		(void)last_exit_code(1, 128 + g_signal_status);
		g_signal_status = 0;
		return (-3);
	}
	if (!line)
	{
		ft_putstr_fd("warning: here-document delimited by "
			"end-of-file (wanted `", 2);
		write(2, delimiter, ft_strlen(delimiter));
		write(2, "')\n", sizeof("')\n") - 1);
	}
	free(in_redir->filename);
	in_redir->filename = file_name;
	(void)last_exit_code(1, 0);
	return (0);
}

/*
* the return values are identical to those of open_heredoc_files():
*/
static
int	prepare_heredoc_files(t_command *cmd, char ***env)
{
	t_redir *in;
	int		i;
	int		failure_flag;

	failure_flag = 0;
	in = cmd->in_redir;
	i = 1;
	while (in)
	{
		if (in->type == REDIR_HEREDOC)
		{
			failure_flag = open_heredoc_files(in, in->filename, i, env);
			if (failure_flag)
				return (failure_flag);
			i++;
		}
		in = in->next;
	}
	return (0);
}

/*
* returns true if the user demands at most 16 here documents
*/
static
int	is_n_heredocs_reasonable(t_command *cmd)
{
	size_t		i;
	t_redir		*in;
	t_command	*head;

	i = 0;
	head = cmd;
	in = cmd->in_redir;
	while (cmd && i <= 16)
	{
		while (in)
		{
			if (in->type == REDIR_HEREDOC)
				i++;
			in = in->next;
		}
		cmd = cmd->next;
	}
	if (i > 16)
		return (0);
	return (1);
}

/*
* return values: 0 or 1. A detected malloc() failure, or an unreasonable amount
* of heredoc files (over 16) cleans up and exits.
* returns 0 in case SIGINT was intercepted in the heredoc or if open() failed;
* The Minishell loop shall continue in that case, and only the 'cmd' list should
* be freed, not env().
* Upon success, handle_heredocs() returns 1
*/
int	handle_heredocs(t_command **cmd, char ***env, t_command *current)
{
	int	failure_flag;

	if (!is_n_heredocs_reasonable(*cmd))
	{
		write(2, "maximum here-document count exceeded\n",
			sizeof("maximum here-document count exceeded\n") - 1);
		cleanup_heredocs(*cmd);
		free_rest(NULL, cmd, env);
		rl_clear_history();
		exit (2);
	}
	while (current)
	{
		if (current->in_redir)
		{
			failure_flag = prepare_heredoc_files(current, env);
			if (failure_flag == -1)
			{
				cleanup_heredocs(*cmd);
				free_cmd(cmd);
				return (0);
			}
			if (failure_flag == -2)
			{
				cleanup_heredocs(*cmd);
				free_rest(NULL, cmd, env);
				rl_clear_history();
				write(2, ALLOCATION_FAILURE, sizeof(ALLOCATION_FAILURE) - 1);
				exit (last_exit_code(1, 1));
			}
			if (failure_flag == -3)
			{
				cleanup_heredocs(*cmd);
				free_cmd(cmd);
				return (0);
			}
		}
		current = current->next;
	}
	return (1);
}
