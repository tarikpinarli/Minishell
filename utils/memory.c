/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:30:48 by tpinarli          #+#    #+#             */
/*   Updated: 2025/06/05 19:05:53 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	free_tokens(t_token **tokens);

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

static void	free_tokens(t_token **tokens)
{
	int	i;

	i = 0;
	while ((*tokens)[i].str != NULL)
	{
		free((*tokens)[i].str);
		i++;
	}
	free(*tokens);
	*tokens = NULL;
}

void	free_all(char **input, t_token **tokens, t_command **cmd)
{
	if (input && *input)
	{
		free(*input);
		*input = NULL;
	}
	if (tokens && *tokens)
		free_tokens(tokens);
	if (cmd && *cmd)
		free_cmd(cmd);
}

void	free_tokens_and_input(t_token **tokens, char **input)
{
	if (input && *input)
	{
		free(*input);
		*input = NULL;
	}
	if (tokens && *tokens)
		free_tokens(tokens);
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

// NOTE: new version, which unlinks each in_redir list within a single command
// node - this is supposed to clean up at the end of each and every pipeline!
int	cleanup_heredocs(t_redir *current_in_redir)
{
	char	*heredoc_file_name;
	char	*heredoc_number;
	int		i;

	i = 1;
	while (current_in_redir)
	{
		if (current_in_redir->type == REDIR_HEREDOC)
		{
			heredoc_number = ft_itoa(i);
			if (!heredoc_number)
				return (0);
			heredoc_file_name = ft_strjoin("heredoc_", heredoc_number);
			free(heredoc_number);
			if (!heredoc_file_name)
				return (0);
			unlink(heredoc_file_name);
			free(heredoc_file_name);
			i++;
		}
		current_in_redir = current_in_redir->next;
	}
	return (1);
}
