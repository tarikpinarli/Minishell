/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykadosh <ykadosh@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 14:20:46 by ykadosh           #+#    #+#             */
/*   Updated: 2025/06/08 08:26:28 by ykadosh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
* return values:
* -1 upon failure of open()
* -2 upon malloc() failure
* -3 upon interception of SIGINT during heredoc's readline
* 0, otherwise
*/
static int	run_readline(t_heredoc *heredoc, char ***env)
{
	rl_event_hook = &heredoc_signal_hook;
	heredoc->line = readline("\001\033[1m\002heredoc> \001\033[0m\002");
	if (g_signal_status == SIGINT || !heredoc->line
		|| !ft_strcmp(heredoc->line, heredoc->in->filename))
		return (-1);
	if (!heredoc->in->is_heredoc_delimiter_quoted)
	{
		if (check_if_str_contains_vars_to_expand(heredoc->line))
			if (!rebuild_expandable_heredoc_line(&heredoc->line, env))
				return (-2);
	}
	if (heredoc->line)
	{
		write(heredoc->fd, heredoc->line, ft_strlen(heredoc->line));
		free(heredoc->line);
		heredoc->line = NULL;
	}
	write(heredoc->fd, "\n", 1);
	return (0);
}

/*
* NOTE: At the start of the heredoc process, the delimiter indicating EOF for
* our here-document is saved in in_redir->filename (which the heredoc struct
* also points at, through 'heredoc->in->filename'). Once the readline flow for
* each file is complete, however - whether successfully or unsuccessfully, as
* long as a temporary heredoc file has been opened - that string is freed and
* is replaced by the name of the file that has been created, allowing
* cleanup_heredocs() to unlink that file with ease.
*/
static int	readline_aftermath(t_heredoc *heredoc)
{
	if (g_signal_status == SIGINT)
	{
		(void)last_exit_code(1, 128 + g_signal_status); // WARN: check this on Linux, is it necessary in case of SIGINT during heredocs???? on Mac it returns 1 which is odd
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
	if (heredoc->line)
		free(heredoc->line);
	close(heredoc->fd);
	free(heredoc->in->filename);
	heredoc->in->filename = heredoc->heredoc_filename;
	(void)last_exit_code(1, 0);
	return (0);
}

/*
* return values: identical to launch_readline_flow()
*/
static int	execute_heredoc_readline_loop(t_heredoc *heredoc, char ***env)
{
	int	flag;

	while (1)
	{
		flag = 0;
		flag = run_readline(heredoc, env);
		if (flag == -1)
			break ;
		if (flag == -2)
		{
			prepare_heredoc_cleaning(heredoc);
			return (-2);
		}
	}
	if (readline_aftermath(heredoc) == -3)
		return (-3);
	return (0);
}

/*
* Return values:
* -1: failure to open() a temporary file
* -2: malloc() has failed, when concatenating a file name or when attempting
*  to rebuild readline's returned string (necessary when there are variables to
*  expand, in case where the string delimiter following the heredoc redirection
*  token "<<" does not contain any quotes). If there are quotes, no expansion
*  is taking place - just like on bash.
*  -3: SIGINT is intercepted during the heredoc readline loop
*  0: otherwise
*/
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

/*
* return values: 0 or 1. A detected malloc() failure, or an unreasonable amount
* of heredoc files (over 16) cleans up and exits.
* returns 0 in case SIGINT was intercepted during heredoc's readline or if
* open() failed; in those cases, the Minishell loop shall continue, and only
* the 'cmd' list should be freed - 'env' should not, as it should remain
* accessible for the user.
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
