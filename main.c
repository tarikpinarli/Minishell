/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 20:40:00 by ykadosh           #+#    #+#             */
/*   Updated: 2025/05/26 13:39:42 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*handle_input(void)
{
	char	*input;

	input = readline("minishell$ ");
	if (!input)
	{
		printf("exit\n");
		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);
		return (NULL);
	}
	if (input[0])
		add_history(input);
	else
	{
		free(input);
		return (NULL);
	}
	return (input);
}

static void	process_command(char *input, char ***env)
{
	t_token		*tokens;
	t_command	*cmd;

	tokens = NULL;
	cmd = NULL;
	if (tokenize(input, &tokens) == -1)
		return ;
	expand_tokens(tokens, input);
	merge_tokens(tokens, input);
	cmd = parse_tokens(tokens);
	free_tokens(tokens);
	if (!cmd)
	{
		free_all(input, tokens, cmd);
		return ;
	}
	if (cmd->next)
		execute_pipeline(cmd, env);
	else
		exec_command(cmd, env);
	cleanup_heredocs(cmd);
	free_all(input, tokens, cmd);
}

int	main(int argc, char **argv, char **envp)
{
	char	**env;
	char	*input;

	(void)argc;
	(void)argv;
	env = copy_env(envp);
	setup_signals();
	while (1)
	{
		input = handle_input();
		if (!input)
			break ;
		process_command(input, &env);
	}
	free_env(env);
	rl_clear_history();
	return (0);
}
