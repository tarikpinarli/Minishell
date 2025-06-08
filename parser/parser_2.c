/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_2.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/08 13:04:04 by tpinarli          #+#    #+#             */
/*   Updated: 2025/06/08 13:32:43 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
 * Allocates and initializes a redirection node with the given type and filename.
 * If the redirection is a quoted heredoc (<< with quotes), sets the appropriate
 * flag.
 *
 * Returns:
 * - A pointer to the newly created redirection node
 * - NULL if memory allocation fails
 */
t_redir	*create_redir(t_redir_type type, char *filename, t_quote_type quote)
{
	t_redir	*redir;

	redir = (t_redir *)ft_calloc(1, sizeof(t_redir));
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
	if (type == REDIR_HEREDOC && quote != QUOTE_NONE)
		redir->is_heredoc_delimiter_quoted = 1;
	redir->next = NULL;
	return (redir);
}

/*
 * Appends a new redirection node to the appropriate redirection list
 * (input or output) of the given command based on the redirection type.
 *
 * If the list is empty, the new redirection becomes the head.
 * Otherwise, it is added to the end of the linked list.
 */
void	append_redir(t_redir_type type, t_redir *new_redir, t_command *current)
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

/*
 * Appends a new argument to the existing argv array by creating
 * a new array with additional space, copying the existing arguments,
 * and adding the new one at the end.
 *
 * Automatically NULL-terminates the resulting array.
 * Frees the original argv on success or failure to prevent memory leaks.
 *
 * Returns:
 * - The newly allocated argv array with the new argument appended
 * - NULL if memory allocation fails (after cleaning up)
 */
char	**argv_add(char **argv, char *new_arg)
{
	int		count;
	int		i;
	char	**new_argv;

	if (!new_arg)
		return (argv);
	count = 0;
	i = 0;
	while (argv && argv[count])
		count++;
	new_argv = (char **)malloc(sizeof(char *) * (count + 2));
	if (!new_argv)
		return (free_two_dimensional_array(&argv), NULL);
	while (i < count)
	{
		new_argv[i] = argv[i];
		i++;
	}
	new_argv[count] = ft_strdup(new_arg);
	if (!new_argv[count])
		return (free(new_argv), free_two_dimensional_array(&argv), NULL);
	new_argv[count + 1] = NULL;
	free(argv);
	return (new_argv);
}
