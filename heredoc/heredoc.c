/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykadosh <ykadosh@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 14:20:46 by ykadosh           #+#    #+#             */
/*   Updated: 2025/06/08 03:11:58 by ykadosh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// FIXME: original function notes, still unincorporated:
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
static int	open_file_and_handle_error(t_heredoc *heredoc, size_t *i)
{
	heredoc->fd = open(heredoc->heredoc_filename,
					O_WRONLY | O_CREAT | O_EXCL | O_CLOEXEC, S_IRUSR | S_IWUSR);
	if (heredoc->fd < 0)
	{
		if (errno == EEXIST)
		{
			(*i)++;
			free(heredoc->heredoc_filename);
			return (-1);  // continue ;
		}
		else
		{
			perror(heredoc->heredoc_filename);
			free(heredoc->heredoc_filename);
			return (-2);
		}
	}
	return (0); // break ;
}

static char	*generate_filename(t_heredoc *heredoc, size_t i)
{
	char	*file_number;

	file_number = NULL;
	heredoc->heredoc_filename = NULL;
	file_number = ft_itoa(i);
	if (!file_number)
		return (NULL);
	heredoc->heredoc_filename = ft_strjoin("heredoc_", file_number);
	free(file_number);
	if (!heredoc->heredoc_filename)
		return (NULL);
	return (heredoc->heredoc_filename);
}

/*
* return values:
* -1 in case of an open() failure - which is not related to an already
* existing file of the same name, since this program keeps trying until it finds
* an available file name.
* -2 in case of malloc() failure
* 0 if everything is smooth
*/
static int	open_temp_file(t_heredoc *heredoc, size_t *i)
{
	int	ret;

	while (1)
	{
		heredoc->heredoc_filename = generate_filename(heredoc, *i);
		if (!heredoc->heredoc_filename)
			return (-2);
		ret = open_file_and_handle_error(heredoc, i);
		if (ret == -1)
			continue;
		else if (ret == -2)
			return (-1);
		break ;
	}
	return (0);
}

static int	run_readline(t_heredoc *heredoc, char ***env)
{
	rl_event_hook = &heredoc_signal_hook;
	heredoc->line = readline("\001\033[1m\002heredoc> \001\033[0m\002");
	if (g_signal_status == SIGINT || !heredoc->line
		|| !ft_strcmp(heredoc->line, heredoc->in->filename))
		return (-1); // break ; from while (1) loop
	if (!heredoc->in->is_heredoc_delimiter_quoted)
	{
		if (check_if_str_contains_vars_to_expand(heredoc->line))
			if (!rebuild_expandable_heredoc_line(&heredoc->line, env))
				return (-2); // malloc() failed
	}
	if (heredoc->line) // line might be NULL after rebuild_expandable_heredoc_line() (if a var expanded to nothing), and if we don't check for it, ft_strlen will segfault!
	{
		write(heredoc->fd, heredoc->line, ft_strlen(heredoc->line));
		free(heredoc->line);
		heredoc->line = NULL; // added just now...
	}
	write(heredoc->fd, "\n", 1); // this is also needed if a variable expanded to nothing - the heredoc on bash still displays a newline in that case; so this command should not appear in the above "if (line)" control structure.
	return (0);
}

static int	readline_aftermath(t_heredoc *heredoc)
{
	if (g_signal_status == SIGINT)
	{
		(void)last_exit_code(1, 128 + g_signal_status); // WARN: check this on Linux, is it necessary in case of SIGINT during heredocs????
		g_signal_status = 0;
		free(heredoc->in->filename);
		heredoc->in->filename = heredoc->heredoc_filename;
		close(heredoc->fd);
		return (-3);
	}
	if (!heredoc->line)
	{
		ft_putstr_fd("warning: here-document delimited by "
			"end-of-file (wanted `", 2);
		write(2, heredoc->in->filename, ft_strlen(heredoc->in->filename));
		write(2, "')\n", sizeof("')\n") - 1);
	}
	if (heredoc->line)  // needed here if the delimiter comparison was successful (or if by any chance SIGINT was intercepted and readline already allocated the line...)
		free(heredoc->line);
	close(heredoc->fd);
	free(heredoc->in->filename);
	heredoc->in->filename = heredoc->heredoc_filename;
	(void)last_exit_code(1, 0);
	return (0);
}

static void	prepare_heredoc_cleaning(t_heredoc *heredoc)
{
	free(heredoc->in->filename);
	heredoc->in->filename = heredoc->heredoc_filename;
}

static void	init_heredoc_struct(t_heredoc *heredoc, t_command *cmd, int *flag)
{
	ft_bzero(heredoc, sizeof(t_heredoc));
	heredoc->in = cmd->in_redir;
	*flag = 0;
}

static int	execute_heredoc_readline_loop(t_heredoc *heredoc, char ***env)
{
	int	flag;

	while (1)
	{
		flag = 0;
		flag = run_readline(heredoc, env);
		if (flag == -1)
			break ;
		if (flag == -2) // malloc failure. heredoc_filename hasn't been assigned
		{
			prepare_heredoc_cleaning(heredoc);
			return (-2);
		}
	}
	if (readline_aftermath(heredoc) == -3)
		return (-3); // SIGINT was intercepted during the readline() loop
	return (0);
}

static int	launch_readline_flow(t_command *cmd, char ***env, size_t *i)
{
	t_heredoc	heredoc;
	int			flag;
	
	init_heredoc_struct(&heredoc, cmd, &flag);
	while (heredoc.in)
	{
		if (heredoc.in->type == REDIR_HEREDOC)
		{
			flag = open_temp_file(&heredoc, i);
			if (flag)
				return (flag);
			flag = execute_heredoc_readline_loop(&heredoc, env);
			if (flag < 0)
				return (flag);
			(*i)++;
		}
		heredoc.in = heredoc.in->next;
	}
	return (0);
}

static void	abort_heredocs_sigint_detected_or_open_failed(t_command **cmd)
{
	cleanup_heredocs(*cmd);
	free_cmd(cmd);
}

static void	exit_heredocs_malloc_failure(t_command **cmd, char ***env)
{
	cleanup_heredocs(*cmd);
	free_rest(NULL, cmd, env);
	rl_clear_history();
	write(2, ALLOCATION_FAILURE, sizeof(ALLOCATION_FAILURE) - 1);
	exit (last_exit_code(1, 1));
}

static void	exit_too_many_heredocs(t_command **cmd, char ***env)
{
	write(2, "maximum here-document count exceeded\n",
		sizeof("maximum here-document count exceeded\n") - 1);
	cleanup_heredocs(*cmd);
	free_rest(NULL, cmd, env);
	rl_clear_history();
	exit (2);
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
	int		failure_flag;
	size_t	i;

	if (!is_n_heredocs_reasonable(*cmd))
		exit_too_many_heredocs(cmd, env);
	i = 1;
	while (current)
	{
		if (current->in_redir)
		{
			failure_flag = launch_readline_flow(current, env, &i);
			if (failure_flag == -1 || failure_flag == -3)
			{
				abort_heredocs_sigint_detected_or_open_failed(cmd);
				return (0);
			}
			if (failure_flag == -2)
				exit_heredocs_malloc_failure(cmd, env);
		}
		current = current->next;
	}
	return (1);
}
