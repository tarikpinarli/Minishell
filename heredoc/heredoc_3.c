/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_3.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykadosh <ykadosh@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/08 03:22:47 by ykadosh           #+#    #+#             */
/*   Updated: 2025/06/08 08:26:08 by ykadosh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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
			return (-1);
		}
		else
		{
			perror(heredoc->heredoc_filename);
			free(heredoc->heredoc_filename);
			return (-2);
		}
	}
	return (0);
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
int	open_temp_file(t_heredoc *heredoc, size_t *i)
{
	int	ret;

	while (1)
	{
		heredoc->heredoc_filename = generate_filename(heredoc, *i);
		if (!heredoc->heredoc_filename)
			return (-2);
		ret = open_file_and_handle_error(heredoc, i);
		if (ret == -1)
			continue ;
		else if (ret == -2)
			return (-1);
		break ;
	}
	return (0);
}

/*
* returns true if the user requests at most sixteen here documents: mimicking
* bash's cap of sixteen heredocs per command
*/
int	is_n_heredocs_reasonable(t_command *cmd)
{
	size_t		i;
	t_redir		*in;

	i = 0;
	while (cmd && i <= 16)
	{
		in = cmd->in_redir;
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
