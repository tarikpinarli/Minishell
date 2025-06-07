/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draft_heredoc.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykadosh <ykadosh@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 14:20:46 by ykadosh           #+#    #+#             */
/*   Updated: 2025/06/07 23:22:39 by ykadosh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
* return values:
* -1 upon failure of open()
* -2 upon malloc() failure
* -3 upon interception of SIGINT during heredoc's readline
* 0, otherwise
*
* NOTE: at the start of this function, 'delim' is in_redir->filename;
* At its end, if the heredoc temporary file was successfully opened, it is freed
* and replaced by the name of the file that has been created, allowing
* cleanup_heredocs() to unlink that file with ease
*/
static int	open_temp_files(t_redir *in_redir, char *delim, int i, char ***env)
{
	char	*file_number;
	char	*heredoc_filename;
	char	*line;
	int		fd;

	while (1)
	{
		file_number = NULL;
		heredoc_filename = NULL;
		file_number = ft_itoa(i);
		if (!file_number)
			return (-2);
		heredoc_filename = ft_strjoin("heredoc_", file_number);
		free(file_number);
		if (!heredoc_filename)
			return (-2);
		fd = open(heredoc_filename, O_WRONLY | O_CREAT | O_EXCL | O_CLOEXEC, // makes the heredoc file be created ONLY if the file does not exist already.
			S_IRUSR | S_IWUSR); // This gives the file: 0600 permissions.
		if (fd < 0)
		{
			if (errno == EEXIST) // if the file name already exists for another file, we are incrementing i, and trying this loop again with another name.
			{
				i++;
				free(heredoc_filename);
				continue;
			}
			else
			{
				perror(heredoc_filename);
				free(heredoc_filename);
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
		if (!line || !ft_strcmp(line, delim) || g_signal_status == SIGINT)
			break ;
		if (!in_redir->is_heredoc_delimiter_quoted)
		{
			if (check_if_str_contains_vars_to_expand(line))
				if (!rebuild_expandable_heredoc_line(&line, env))
					return (-2);
		}
		if (line) // line might be NULL after rebuild_expandable_heredoc_line() (if a var expanded to nothing), and if we don't check for it, ft_strlen will segfault!
		{
			write(fd, line, ft_strlen(line));
			free(line);
		}
		write(fd, "\n", 1); // this is also needed if a variable expanded to nothing - the heredoc on bash still displays a newline in that case; so this command should not appear in the above "if (line)" control structure.
	}
	if (g_signal_status == SIGINT)
	{
		(void)last_exit_code(1, 128 + g_signal_status); // WARN: check this on Linux, is it necessary in case of SIGINT during heredocs????
		g_signal_status = 0;
		free(in_redir->filename);
		in_redir->filename = heredoc_filename;
		close(fd);
		return (-3);
	}
	if (!line)
	{
		ft_putstr_fd("warning: here-document delimited by "
			"end-of-file (wanted `", 2);
		write(2, delim, ft_strlen(delim));
		write(2, "')\n", sizeof("')\n") - 1);
	}
	if (line)  // needed here if the delimiter comparison was successful (or if by any chance SIGINT was intercepted and readline already allocated the line...)
		free(line);
	close(fd);
	free(in_redir->filename);
	in_redir->filename = heredoc_filename;
	(void)last_exit_code(1, 0);
	return (0);
}

/*
* O_CREAT and O_EXCL would cause open() to fail if a file already exists under
* an idnetical name to 'heredoc_filename' parameter, setting errno to EEXIST.
* In that case, this function increments i, and the caller's loop will call this
* function once again but with a different name, until a new file is created
* and nothing gets overwritten.
* Reagarding file permissions: the heredocs are just temporary files, which
* should not be accessible to others other than this program; Therefore, the
* open() call assigns permissions: 600.
*/
static int	open_file_and_handle_error(int *fd, char *heredoc_filename, int *i)
{
	fd = open(heredoc_filename, O_WRONLY | O_CREAT | O_EXCL | O_CLOEXEC,
		S_IRUSR | S_IWUSR);
	if (fd < 0)
	{
		if (errno == EEXIST)
		{
			(*i)++;
			free(heredoc_filename);
			return (-1);
			// continue;
		}
		else
		{
			perror(heredoc_filename);
			free(heredoc_filename);
			return (-2);
		}
	}
	returns (0);
	// break ;
}

static char	*generate_filename(char *heredoc_filename, int i)
{
	char	*file_number;

	file_number = NULL;
	heredoc_filename = NULL;
	file_number = ft_itoa(i);
	if (!file_number)
		return (NULL);
	heredoc_filename = ft_strjoin("heredoc_", file_number);
	free(file_number);
	if (!heredoc_filename)
		return (NULL);
	return (heredoc_filename);
}

// TODO:
static int	heredoc_readline_loop(char *line, char *delim, char ***env, int fd)
{
	while (1)
	{
		rl_event_hook = &heredoc_signal_hook;
		line = readline("\001\033[1m\002heredoc> \001\033[0m\002");
		if (!line || !ft_strcmp(line, delim) || g_signal_status == SIGINT)
			break ;
		if (!in_redir->is_heredoc_delimiter_quoted)
		{
			if (check_if_str_contains_vars_to_expand(line))
				if (!rebuild_expandable_heredoc_line(&line, env))
					return (-2);
		}
		if (line) // line might be NULL after rebuild_expandable_heredoc_line() (if a var expanded to nothing), and if we don't check for it, ft_strlen will segfault!
		{
			write(fd, line, ft_strlen(line));
			free(line);
		}
		write(fd, "\n", 1); // this is also needed if a variable expanded to nothing - the heredoc on bash still displays a newline in that case; so this command should not appear in the above "if (line)" control structure.
	}

/*
* return values:
* -1 in case of an open() failure - which is not related to an already
* existing file of the same name, since this program keeps trying until it finds
* an available file name.
* -2 in case of malloc() failure
* returns....?????????????????????
*/
static int	open_temp_file(t_redir *in_redir, char *delim, int *i)
{
	char	*heredoc_filename;
	int		fd;
	int		ret;

	while (1)
	{
		heredoc_filename = generate_filename(heredoc_filename, *i);
		if (!heredoc_filename)
			return (-2);

		ret = open_file_and_handle_error(&fd, heredoc_filename, i);
		if (ret == -1)
			continue;
		else if (ret == -2)
			return (-1);
		break ;

		/*
		fd = open(heredoc_filename, O_WRONLY | O_CREAT | O_EXCL | O_CLOEXEC, // makes the heredoc file be created ONLY if the file does not exist already.
			S_IRUSR | S_IWUSR); // This gives the file: 0600 permissions.
		if (fd < 0)
		{
			if (errno == EEXIST) // if the file name already exists for another file, we are incrementing i, and trying this loop again with another name.
			{
				i++;
				free(heredoc_filename);
				continue;
			}
			else
			{
				perror(heredoc_filename);
				free(heredoc_filename);
				return (-1);
			}
		}
		break ;
		*/
	}
	return (0);
}


/*
* the return values are identical to those of open_temp_files(): ???????
*/
static int	run_heredoc_files(t_command *cmd, char ***env)
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
			failure_flag = open_temp_file(in, in->filename, &i, env);
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
static int	is_n_heredocs_reasonable(t_command *cmd)
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
			failure_flag = run_heredoc_files(current, env);
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
