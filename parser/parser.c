/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 15:16:33 by tpinarli          #+#    #+#             */
/*   Updated: 2025/04/12 18:05:04 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_redir	*create_redir(t_redir_type type, char *filename)
{
	t_redir *redir = malloc(sizeof(t_redir));
	if (!redir)
		return (NULL);
	redir->type = type;
	redir->filename = ft_strdup(filename);
	redir->next = NULL;
	return redir;
}

void append_redir(t_redir **redir_list, t_redir *new_redir)
{
	if (!*redir_list)
		*redir_list = new_redir;
	else
	{
		t_redir *last = *redir_list;
		while (last->next)
			last = last->next;
		last->next = new_redir;
	}
}

char **argv_add(char **argv, char *new_arg)
{
	int		count = 0;
	char	**new_argv;

	// Count current size
	while (argv && argv[count])
		count++;

	new_argv = malloc(sizeof(char *) * (count + 2));
	if (!new_argv)
		return (NULL);

	for (int i = 0; i < count; i++)
		new_argv[i] = argv[i];

	new_argv[count] = ft_strdup(new_arg);
	new_argv[count + 1] = NULL;

	free(argv); // free old array pointer, not the strings
	return (new_argv);
}



t_command *parse_tokens(t_token *tokens)
{
	t_command	*head = NULL;
	t_command	*current = NULL;
	t_command	*new_cmd;
	int			i = 0;

	while (tokens[i].str)
	{
		// Pipe görürsek → yeni komuta geç
		if (!ft_strcmp(tokens[i].str, "|"))
		{
			if (!current)
			{
				printf("syntax error near unexpected token `|`\n");
				return NULL;
			}
			new_cmd = ft_calloc(1, sizeof(t_command));
			current->next = new_cmd;
			current = new_cmd;
			i++;
			continue;
		}

		// Komut zincirinin başlangıcı
		if (!current)
		{
			current = ft_calloc(1, sizeof(t_command));
			head = current;
		}

		// Redirection kontrolü
		if (!ft_strcmp(tokens[i].str, "<") && tokens[i + 1].str)
			append_redir(&current->in_redir, create_redir(REDIR_IN, tokens[++i].str));
		else if (!ft_strcmp(tokens[i].str, "<<") && tokens[i + 1].str)
			append_redir(&current->in_redir, create_redir(REDIR_HEREDOC, tokens[++i].str));
		else if (!ft_strcmp(tokens[i].str, ">") && tokens[i + 1].str)
			append_redir(&current->out_redir, create_redir(REDIR_OUT, tokens[++i].str));
		else if (!ft_strcmp(tokens[i].str, ">>") && tokens[i + 1].str)
			append_redir(&current->out_redir, create_redir(REDIR_APPEND, tokens[++i].str));
		else
		{
			// Normal argüman: strip + expand
			char *clean = strip_quotes(tokens[i].str);

			if (tokens[i].quote == QUOTE_SINGLE)
				current->argv = argv_add(current->argv, clean);
			else
			{
				char *expanded = expand_variables(clean);
				current->argv = argv_add(current->argv, expanded);
				free(expanded);
			}
			free(clean);
		}
		i++;
	}
	return head;
}
