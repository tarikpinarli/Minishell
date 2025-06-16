/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:30:48 by tpinarli          #+#    #+#             */
/*   Updated: 2025/06/08 14:54:12 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	free_two_dimensional_array(char ***arr)
{
	char	**p_arr;
	int		i;

	if (!arr || !*arr)
		return ;
	p_arr = *arr;
	i = 0;
	while (p_arr[i])
	{
		free(p_arr[i]);
		i++;
	}
	free(*arr);
	*arr = NULL;
}

static void	free_redir_list(t_redir **redir)
{
	t_redir	*p_redir;
	t_redir	*next;

	if (redir && *redir)
	{
		p_redir = *redir;
		while (p_redir)
		{
			next = p_redir->next;
			free(p_redir->filename);
			p_redir->filename = NULL;
			free(p_redir);
			p_redir = next;
		}
		*redir = NULL;
	}
}

void	free_cmd(t_command **cmd)
{
	t_command	*next;
	t_command	*current;

	if (cmd && *cmd)
	{
		current = *cmd;
		while (current)
		{
			next = current->next;
			free_two_dimensional_array(&current->argv);
			free_redir_list(&current->in_redir);
			free_redir_list(&current->out_redir);
			free(current);
			current = next;
		}
		*cmd = NULL;
	}
}

/*
* frees the heap allocated strings in the tokens' array of structs, from index
* 'k' onwards. Strings in indices lower than k are not freed by this function.
*/
void	free_deprecated_strings(t_token *tokens, size_t k)
{
	if (tokens)
	{
		while (tokens[k].str)
		{
			free(tokens[k].str);
			tokens[k].str = NULL;
			k++;
		}
	}
}

/*
* cleans up all the heredoc files, no matter in which pipeline they are
* (as long as a pointer to the head of the 'cmd' list is passed)
*/
int	cleanup_heredocs(t_command *cmd)
{
	t_redir	*current;

	while (cmd)
	{
		current = cmd->in_redir;
		while (current)
		{
			if (current->type == REDIR_HEREDOC)
				unlink(current->filename);
			current = current->next;
		}
		cmd = cmd->next;
	}
	return (1);
}
