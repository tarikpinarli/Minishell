/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:30:48 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/12 15:16:58 by ykadosh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	ft_free_split(char **arr)
{
	int	i = 0;

	if (!arr)
		return ;
	while (arr[i])
	{
		free(arr[i]);
		i++;
	}
	free(arr);
}

static void	free_argv(char **argv)
{
	int	i;

	i = 0;
	if (!argv)
		return;
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

static void	free_input(char *input)
{
	if (input)
		free(input);
}

static void	free_tokens(t_token *tokens)
{
	int		i;

	i = 0;
	if (tokens)
	{
		while (tokens[i].str != NULL)
		{
			free(tokens[i].str);
			i++;	
		}
		free(tokens);
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

void	free_deprecated_strings(t_token *tokens, size_t k)
{
	while (tokens[k].str)
	{
		free(tokens[k].str);
		tokens[k].str = NULL;
		k++;
	}
}

void	free_all(char *input, t_token *tokens, t_command *cmd)
{
	free_input(input);
	free_tokens(tokens);
	free_cmd(cmd);
}
