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

static t_redir_type	get_redirection_id(const char *str);

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
	t_redir	*temp;
	t_redir *last_node;

	temp = NULL;
	last_node = NULL;
	if (type == REDIR_IN || type == REDIR_HEREDOC)
		temp = current->in_redir;
	else
		temp = current->out_redir;
	if (!temp)
		temp = new_redir;
	else
	{
		last_node = temp->next;
		while (last_node->next)
			last_node = last_node->next;
		last_node->next = new_redir;
	}
	// WARN: this last block is just for printf() debugging!!
	if (last_node->next)
		printf("ERROR occured in the parser(): appended node's next pointer is not NULL!!!\n\n");
	if (last_node->type != type)
		printf("ERROR occured in the parser(): appended node's type is not compatible with the type it is supposed to be!!\n\n");
}

/*
 * older version:
void append_redir(t_redir **redir_list, t_redir *new_redir)
{
	t_redir *last;

	if (!new_redir)
	{
		if (*redir_list)
			free_redir_l
		return ; // to protect the malloc_failure
	}
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
*/

// TODO: protect this function from empty strings (such as invalid variables!)
char **argv_add(char **argv, char *new_arg)
{
	int		count;
	int		i;
	char	**new_argv;

<<<<<<< HEAD
	if (!new_arg || new_arg[0] == '\0')
		return (argv);
	// Count current size
	count = 0;
	i = 0;
	while (argv && argv[count])
		count++;

	new_argv = malloc(sizeof(char *) * (count + 2));
	if (!new_argv) // FIXME: is this sufficient? Is there a need to free() something first?
		return (NULL);

	while (i < count)
	{
		new_argv[i] = argv[i];
		i++;
	}

	new_argv[count] = ft_strdup(new_arg);
	// FIXME: needs protection?
	new_argv[count + 1] = NULL;

	free(argv); // free old array pointer
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
	uint8_t			pipe_flag;

	head = NULL;
	current = NULL;
	new_cmd = NULL;
	new_redir = NULL;
	pipe_flag = 0;
	i = 0;
	while (tokens[i].str)
	{
		if (!ft_strcmp(tokens[i].str, "|"))
		{
			pipe_flag = 1;
			if (!current || (tokens[i] && (!ft_strcmp(tokens[i].str, "|")))) // WARN: I think that our Minishell might not be handling cases where there would be valid arguments but then at their end there would be two or more consecutive pipes - which bash interprets as the next code of block (to be checked). So should I add here: || current->
			{
				printf("syntax error near unexpected token `|'\n");
				return (NULL);
			}
			new_cmd = (t_command *)ft_calloc(1, sizeof(t_command));
			if (!new_cmd)
			{
				// TODO: ?????
				// free_cmd(current);	// ??? necessary? study well the list to understand what is being allocated...and study the free_cmd() function, I am not quite sure to understand the **argv element!
				free_all(input, tokens, head); // double check if it is indeed 'head' that should be passed here!
				exit (last_exit_code(1, 1));
			}

			current->next = new_cmd;
			current = new_cmd;
			new_cmd = NULL;
			i++;
			if (tokens[i] && (!ft_strcmp(tokens[i].str, "|")))


			continue ; // contnue with next tokens[i].str
		}
		// start of chain
		if (!current)
		{
			current = (t_command *)ft_calloc(1, sizeof(t_command));
			// FIXME: protect? but how?
			
			head = current;
		}
		// Redirection control
		redir_id = get_redirection_id(tokens[i].str);
		if (redir_id)
		{
			i++;
			if (!tokens[i].str)
			{
				(void)printf("syntax error near unexpected token `newline'\n");
				free_all(input, tokens, head); // check that "head" is the correct one to pass to free_all...
				return (NULL);
			}
			new_redir = create_redir(redir_id, tokens[i].str);
			if (!new_redir)  // here we protect the malloc()
			{
				free_all(input, tokens, head);
				exit (last_exit_code(1, 1));
			}
			if (!get_redirection_id(tokens[i].str))
			{
				// our redirection symbol is followed by another redirection symbol. handle this differently?
			}
			else
				append_redir(redir_id, new_redir, current);
		}
		else // TODO: we are here now!
			current->argv = argv_add(current->argv, tokens[i].str);
		i++;
	}
	return (head);
}

/*
* returns the type of redirection that the string, passed as a parameter,
* corresponds to - or REDIR_NONE - if it corresponds to neither.
*/
static t_redir_type	get_redirection_id(const char *str)
{
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
