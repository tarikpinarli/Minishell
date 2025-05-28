/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 20:40:00 by ykadosh           #+#    #+#             */
/*   Updated: 2025/05/28 19:34:29 by ykadosh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <readline/readline.h>

int	main(int argc, char **argv, char **envp)
{
	char		**env;
	char		*input;
	t_token		*tokens;
	t_command	*cmd;

	(void)argc;
	(void)argv;
	env = copy_env(envp);
	while (1)
	{
		if (setup_signal_handling(1) == -1)
			return (-500); // WARN: this is a random value, just to handle the sig function failures. How about simply voiding out the return here? or setting some other return value which is not too random looking.
		input = NULL;
		tokens = NULL;
		cmd = NULL;
		rl_event_hook = &readline_signal_hook;
		input = readline("minishell$ ");
		if (!input)
		{
			printf("exit\n");
			close(STDIN_FILENO);
			close(STDOUT_FILENO);
			close(STDERR_FILENO);
			break ;
		}
		if (input[0])
			add_history(input);
		else  // this is for when the input string is empty - just a null terminator
		{
			free(input);
			input = NULL;
			continue ;
		}
		if (!tokenize(input, &tokens)) // it returns 0 if a quotation mark was left unclosed or if no tokens were counted (whitespace input)
			continue ;
		expand_tokens(tokens, input);
		merge_tokens(tokens, input);

		cmd = parse_tokens(tokens, input);
		if (!cmd)
		{
			// printf("Parsing failed.\n"); // NOTE: are we certain we want this "Parsing failed" message? Because we already output different parsing syntax error messages in parse_tokens(), so removing this should be just fine!
			continue ;
		}
		free_tokens_and_input(&input, &tokens)

		if (cmd && cmd->next) // If there is pipe cmd->next exists
			execute_pipeline(cmd, &env);
		else if (cmd)
			exec_command(cmd, &env); // If its a single command
		cleanup_heredocs(cmd); // WARN: does this not segfault if cmd is NULL?
		free_all(input, tokens, cmd);
	}
	free_env(env);
	rl_clear_history();
	return (0);
}
