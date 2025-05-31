/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 15:16:33 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/29 20:35:47 by ykadosh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static t_redir_type	get_redirection_id(const char *str, t_quote_type quote);

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

void append_redir(t_redir_type type, t_redir *new_redir, t_command *current)
{
	t_redir	**temp;

	temp = NULL;
	if (type == REDIR_IN || type == REDIR_HEREDOC)
		temp = &current->in_redir;
	else
		temp = &current->out_redir;
	if (!*temp)
		*temp = new_redir;
	else
	{
		while (*temp)
			temp = &((*temp)->next);
		*temp = new_redir;
	}
}

char **argv_add(char **argv, char *new_arg)
{
	int		count;
	int		i;
	char	**new_argv;

	if (!new_arg)
		return (argv);
	// Count current size
	count = 0;
	i = 0;
	while (argv && argv[count])
		count++;

	new_argv = (char **)malloc(sizeof(char *) * (count + 2)); // NOTE: why do we add 2? because: 1 for the new_arg string, & 1 for the NULL
	if (!new_argv)
	{
		free_two_dimensional_array(&argv); // it is mandatory to free_argv() here, BEFORE returning NULL and calling free_all()!
		// Otherwise we replace argv with NULL, losing access to the memory that has to be freed!
		return (NULL);
	}
	while (i < count)
	{
		new_argv[i] = argv[i];
		i++;
	}

	new_argv[count] = ft_strdup(new_arg);
	if (!new_argv[count])
	{
		free(new_argv); // frees the pointer, the strings will be freed in the next line
		free_two_dimensional_array(&argv);
		return (NULL);
	}
	new_argv[count + 1] = NULL;
	free(argv); // free old array pointer, no need to free the strings.
	return (new_argv);
}

t_command *parse_tokens(t_token *tokens, char *input)
{
	t_command		*head;
	t_command		*current;
	t_command		*new_cmd;
	t_redir			*new_redir;
	t_redir_type	redir_id;
	int				i;

	head = NULL;
	current = NULL;
	new_cmd = NULL;
	new_redir = NULL;
	i = 0;
	while (tokens[i].str)
	{
		if (!ft_strcmp(tokens[i].str, "|"))
		{
			// TESTING: please TEST this next if statement thorughly! I think it should be good.
			// NOTE: careful when refactoring this if statement: It looks over complicated FOR A REASON:
			// the order of the checks are intended. The check for !current should come BEFORE checking
			// for !tokens[i + 1].str, and the last ft_strcmp check has to come AFTER !tokens[i + 1].str....
			// Also, NOTE: do NOT send tokens[i + 1] into ft_strcmp() if it is NULL.
			if (!current || !tokens[i + 1].str || !ft_strcmp(tokens[i + 1].str, "|"))
			{
				if (!tokens[i + 1].str)
					ft_putendl_fd("syntax error: parsing failed.", 2);
				else
					ft_putendl_fd("syntax error near unexpected token `|'", 2);
				(void)last_exit_code(1, 2);
				free_all(&input, &tokens, &head);
				return (NULL);
			}
			new_cmd = (t_command *)ft_calloc(1, sizeof(t_command));
			if (!new_cmd)
			{
				free_all(&input, &tokens, &head);
				rl_clear_history();
				write(2, ALLOCATION_FAILURE, sizeof(ALLOCATION_FAILURE) - 1);
				exit (last_exit_code(1, 1));
			}
			current->next = new_cmd;
			current = new_cmd;
			new_cmd = NULL;
			i++;
			continue ; // continue with next tokens[i].str
		}
		// start of chain
		if (!current)
		{
			current = (t_command *)ft_calloc(1, sizeof(t_command));
			if (!current)
			{
				free_all(&input, &tokens, &head);
				rl_clear_history();
				write(2, ALLOCATION_FAILURE, sizeof(ALLOCATION_FAILURE) - 1);
				exit (last_exit_code(1, 1));
			}
			head = current;
		}
		// Redirection control
		redir_id = get_redirection_id(tokens[i].str, tokens[i].quote);
		if (redir_id)
		{
			i++;
			if (!tokens[i].str)
			{
				ft_putendl_fd("syntax error near unexpected token `newline'", 2);
				(void)last_exit_code(1, 2);
				free_all(&input, &tokens, &head);
				return (NULL);
			}
			if (get_redirection_id(tokens[i].str, tokens[i].quote))
			{
				ft_putstr_fd("syntax error near unexpected token `", 2);
				write(2, tokens[i].str, ft_strlen(tokens[i].str));
				write(2, "'\n", 2);
				// NOTE: this is quite similar to bash's behaviour,
				// except some cases where it is slightly different:
				// [ <> ] , [ <>> ] , [ <<< ] , [ <<<< ]
				// but those (or some of those) start to behave in even other
				// ways if there are more redirection sybols or valid arguments later;
				// This is beyond the scope of this project. But since there are a lot of different behaviours,
				// but most of them end up being parsing errors, so we can consider printing a more general "parsing failed\n" error instead... What does Tarik think?
				(void)last_exit_code(1, 2);
				free_all(&input, &tokens, &head);
				return (NULL);
			}
			if (!ft_strcmp(tokens[i].str, "|"))
			{
				ft_putendl_fd("syntax error near unexpected token `|'", 2);
				(void)last_exit_code(1, 2);
				free_all(&input, &tokens, &head);
				return (NULL);
			}

			new_redir = create_redir(redir_id, tokens[i].str);
			if (!new_redir)
			{
				free_all(&input, &tokens, &head);
				rl_clear_history();
				write(2, ALLOCATION_FAILURE, sizeof(ALLOCATION_FAILURE) - 1);
				exit (last_exit_code(1, 1));
			}
			append_redir(redir_id, new_redir, current);
		}
		else
		{
			current->argv = argv_add(current->argv, tokens[i].str);
			if (!current->argv)
			{
				free_all(&input, &tokens, &head);
				rl_clear_history();
				write(2, ALLOCATION_FAILURE, sizeof(ALLOCATION_FAILURE) - 1);
				exit (last_exit_code(1, 1));
			}
		}
		i++;
	}
	return (head);
}

/*
* returns the type of redirection that the string, passed as a parameter,
* corresponds to - or REDIR_NONE - if it corresponds to neither.
*/
static t_redir_type	get_redirection_id(const char *str, t_quote_type quote)
{
	if (quote != QUOTE_NONE)
		return (REDIR_NONE);
	if (*str == '<')
	{
		str++;
		if (!*str)
			return (REDIR_IN);
		if (*str == '<' && !*(str + 1))
			return (REDIR_HEREDOC);
	}
	else if (*str == '>')
	{
		str++;
		if (!*str)
			return (REDIR_OUT);
		if (*str == '>' && !*(str + 1))
			return (REDIR_APPEND);
	}
	return (REDIR_NONE);
}
