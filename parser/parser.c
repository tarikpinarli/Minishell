/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 15:16:33 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/19 16:46:07 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_redir	*create_redir(t_redir_type type, char *filename)
{
	t_redir *redir;

	redir = (t_redir *)malloc(sizeof(t_redir));
	if (!redir)
		return (NULL);
	redir->type = type;
	redir->filename = ft_strdup(filename);
	if (!redir->filename)
	{
		free(redir);
		redir = NULL;
		return (NULL);
	}
	redir->next = NULL;
	return (redir);
}

void append_redir(t_redir **redir_list, t_redir *new_redir)
{
	t_redir *last;

	if (!*redir_list)
		*redir_list = new_redir;
	else
	{
		last = *redir_list;
		while (last->next)
			last = last->next;
		last->next = new_redir;
	}
}

char **argv_add(char **argv, char *new_arg)
{
	int		count = 0;
	int		i;
	char	**new_argv;

	if (!new_arg || new_arg[0] == '\0')
		return (argv);
	i = 0;
	while (argv && argv[count])
		count++;

	new_argv = malloc(sizeof(char *) * (count + 2));
	if (!new_argv)
		return (NULL);

	while (i < count)
	{
		new_argv[i] = argv[i];
		i++;
	}

	new_argv[count] = ft_strdup(new_arg);
	new_argv[count + 1] = NULL;

	free(argv); // free old array pointer
	return (new_argv);
}



t_command *parse_tokens(t_token *tokens, char *input)
{
	t_command	*head;
	t_command	*current;
	t_command	*new_cmd;
	int			i;

	head = NULL;
	current = NULL;
	i = 0;
	while (tokens[i].str)
	{
		if (!ft_strcmp(tokens[i].str, "|"))
		{
			if (!tokens[++i].str) // check for leaks
			{
				printf("syntax error near unexpected token `|'\n");
				return (NULL);
			}
			if (!current)
			{
				printf("syntax error near unexpected token `|'\n");
				return (NULL);
			}
			new_cmd = ft_calloc(1, sizeof(t_command));
			if (!new_cmd)
			{
				// TODO: ?????
				// free_cmd(current);	// ??? necessary? study well the list to understand what is being allocated...
				free_all(input, tokens, head); // double check if it is indeed 'head' that should be passed here!
				exit (last_exit_code(1, 1);
			}

			current->next = new_cmd;
			current = new_cmd;
			i++;
			continue; // contnue with next tokens[i].str
		}
		// start of chain
		if (!current)
		{
			current = ft_calloc(1, sizeof(t_command));
			head = current;
		}
		// Redirection control
		if (!ft_strcmp(tokens[i].str, "<") && tokens[i + 1].str)
		{
			// first deal with the create_redir() NULL return from malloc failure.
			// Maybe simply by protecting within append_redir() the second variable
			// - "if (!new_redir)", then free and exit (last_exit_code(1, 1));
			// or something of the like!
			append_redir(&current->in_redir, create_redir(REDIR_IN, tokens[++i].str));
		else if (!ft_strcmp(tokens[i].str, "<<") && tokens[i + 1].str)
			append_redir(&current->in_redir, create_redir(REDIR_HEREDOC, tokens[++i].str));
		else if (!ft_strcmp(tokens[i].str, ">") && tokens[i + 1].str)
			append_redir(&current->out_redir, create_redir(REDIR_OUT, tokens[++i].str));
		else if (!ft_strcmp(tokens[i].str, ">>") && tokens[i + 1].str)
			append_redir(&current->out_redir, create_redir(REDIR_APPEND, tokens[++i].str));
		else
			current->argv = argv_add(current->argv, tokens[i].str);
		i++;
	}
	return (head);
}
