/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_command_list.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykadosh <ykadosh@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/31 11:56:12 by ykadosh           #+#    #+#             */
/*   Updated: 2025/05/31 12:47:24 by ykadosh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	print_argv(char **argv);
static void	print_redir_list(t_redir *redir_list, unsigned int in_or_out);
static char	*print_redirection_type(t_redir_type type);

void	print_command_list(t_command *cmd)
{
	t_command	*current;
	size_t		i;

	if (!cmd)
	{
		printf("No command has been registered.\n\n");
		return ;
	}
	current = cmd;

	i = 0;

	while (current)
	{
		printf("COMMAND NUMBER:	<%zu>:\n", i);
		print_argv(current->argv);
		print_redir_list(current->in_redir, 1);
		print_redir_list(current->out_redir, 0);
		current = current->next;
		i++;
		printf("\n\n\n");
	}
}

static void	print_argv(char **argv)
{
	size_t	i;

	i = 0;
	if (!argv)
	{
		printf("argv holds no arguments");
		return ;
	}
	while (argv[i])
	{
		printf("argv[%zu]:	<%s>\n", i, argv[i]);
		i++;
	}
	printf("\n");
}

// in_or_out: 1 is for IN redirection list, 0 is for OUT redirection list
static void	print_redir_list(t_redir *redir_list, unsigned int in_or_out)
{
	t_redir	*current;
	size_t	i;

	if (!redir_list)
	{
		if (in_or_out)
			printf("IN redirection list is empty\n\n");
		else
			printf("OUT redirection list is empty\n\n");
		return ;
	}
	i = 0;
	current = redir_list;
	while (current)
	{
		if (in_or_out)
			printf("IN ");
		else
			printf("OUT ");
		printf("redirection list n. <%zu>:\n", i);
		printf("type:	<%s>\n", print_redirection_type(current->type));
		printf("filename:	<%s>\n", current->filename);
		current = current->next;
		i++;
	}
	printf("\n\n");
}

static char	*print_redirection_type(t_redir_type type)
{
	if (!type)
		return ("REDIR_NONE");
	else if (type == REDIR_IN)
		return ("REDIR_IN");
	else if (type == REDIR_OUT)
		return ("REDIR_OUT");
	else if (type == REDIR_APPEND)
		return ("REDIR_APPEND");
	else if (type == REDIR_HEREDOC)
		return ("REDIR_HEREDOC");
	else
		return (NULL);
}
