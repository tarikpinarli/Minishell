/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_2.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykadosh <ykadosh@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/08 03:22:40 by ykadosh           #+#    #+#             */
/*   Updated: 2025/06/08 08:26:18 by ykadosh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	prepare_heredoc_cleaning(t_heredoc *heredoc)
{
	free(heredoc->in->filename);
	heredoc->in->filename = heredoc->heredoc_filename;
}

void	init_heredoc_struct(t_heredoc *heredoc, t_command *cmd, int *flag)
{
	ft_bzero(heredoc, sizeof(t_heredoc));
	heredoc->in = cmd->in_redir;
	*flag = 0;
}

void	abort_heredocs_sigint_detected_or_open_failed(t_command **cmd)
{
	cleanup_heredocs(*cmd);
	free_cmd(cmd);
}

void	exit_heredocs_malloc_failure(t_command **cmd, char ***env)
{
	cleanup_heredocs(*cmd);
	free_rest(NULL, cmd, env);
	rl_clear_history();
	write(2, ALLOCATION_FAILURE, sizeof(ALLOCATION_FAILURE) - 1);
	exit (last_exit_code(1, 1));
}

void	exit_too_many_heredocs(t_command **cmd, char ***env)
{
	write(2, "maximum here-document count exceeded\n",
		sizeof("maximum here-document count exceeded\n") - 1);
	cleanup_heredocs(*cmd);
	free_rest(NULL, cmd, env);
	rl_clear_history();
	exit (2);
}
