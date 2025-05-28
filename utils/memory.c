/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:30:48 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/23 20:21:29 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	free_2D_char(char **arr)
{
	int i;

	i = 0;
	while (arr[i])
		free(arr[i++]);
	free(arr);
}

void	ft_free_split(char **arr)
{
	int	i;

	i = 0;
	if (!arr)
		return ;
	while (arr[i])
	{
		free(arr[i]);
		i++;
	}
	free(arr);
}

void	free_env(char **env)
{
	int i = 0;
	while (env[i])
		free(env[i++]);
	free(env);
}

void	free_argv(char **argv)
{
	int	i;

	i = 0;
	if (!argv)
		return ;
	while (argv[i])
	{
		free(argv[i]);
		i++;
	}
	free(argv);
}

static void	free_redir_list(t_redir *redir)
{
	t_redir	*next;

	while (redir)
	{
		next = redir->next;
		free(redir->filename);
		free(redir);
		redir = next;
	}
}

void	free_cmd(t_command *cmd)
{
	t_command	*next;

	while (cmd)
	{
		next = cmd->next;
		free_argv(cmd->argv);
		free_redir_list(cmd->in_redir);
		free_redir_list(cmd->out_redir);
		free(cmd);
		cmd = next;
	}
}

void	free_tokens(t_token *tokens)
{
	int	i;

	i = 0;
	while (tokens[i].str != NULL)
	{
		free(tokens[i].str);
		i++;
	}
	free(tokens);
}

// WARN: MAKE 100% sure that you are passing from main a double pointer,
// freeing the memory properly and resetting to NULL both tokens and input!!!!
// Same for 'cmd'!!!
// UPDATE: this is done, but we HAVE to test it once the project compiles.
void	free_all(char **input, t_token **tokens, t_command **cmd)
{
	if (*input)
	{
		free(*input);
		*input = NULL;
	}
	if (*tokens)
	{
		free_tokens(*tokens);
		*tokens = NULL;
	}
	if (*cmd)
	{
		cleanup_heredocs(*cmd);
		free_cmd(*cmd);
		*cmd = NULL;
	}
}

void	free_deprecated_strings(t_token *tokens, size_t k)
{
	while (tokens[k].str)
	{
		free(tokens[k].str);
		tokens[k].str = NULL;
		k++;
	}
}

void	cleanup_heredocs(t_command *cmd)
{
	t_redir *in;
	char	*heredoc_file_name;
	char	*heredoc_number;
	int		i;

	in = cmd->in_redir;
	i = 1;
	while (in)
	{
		
		if (in->type == REDIR_HEREDOC)
		{
			heredoc_number = ft_itoa(i);
			heredoc_file_name = ft_strjoin("heredoc_", heredoc_number);
			unlink(heredoc_file_name);
			free(heredoc_file_name);
			free(heredoc_number);
			i++;
		}
		in = in->next;
	}
}
