/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 20:40:00 by ykadosh           #+#    #+#             */
/*   Updated: 2025/05/24 15:40:54 by tpinarli         ###   ########.fr       */
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
	if (setup_signals() <= -500)
		return (-500); // WARN: this is a totally random value, just to handle the sig function failures...
	while (1)
	{
		if (setup_signal_handling(1) == -1)
			return (-500); // WARN: this is a random value, just to handle the sig function failures. How about simply voiding out the return here? or setting some other return value which is not too random looking.
		input = NULL;
		tokens = NULL;
		cmd = NULL;
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
			continue ;
		}
		rl_event_hook = &readline_signal_hook;  // TODO: write the function for it.
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
		free_tokens(tokens);
		/*
		size_t	i = 0;
		printf("in MAIN, at the very end of the loop, the tokens are:\n");
		while (tokens[i].str)
		{
			printf("tokens[%zu].str:		<%s>\ntokens[%zu].quote:	<%d>\n"
				"tokens[%zu].line_id:	<%d>\nlength of string:	<%zu>\n\n\n",
				i, tokens[i].str, i, tokens[i].quote, i, tokens[i].line_id,
				ft_strlen(tokens[i].str));
			i++;
		}
		printf("\n\n");
		*/

		//if (is_builtin(cmd->arg[0]))
			// if its a builtin command execute.
		//print_command(cmd);
		if (cmd && cmd->next) // If there is pipe cmd->next exists
			execute_pipeline(cmd, &env);
		else if (cmd)
			exec_command(cmd, &env); // If its a single command
		cleanup_heredocs(cmd);
		free_all(input, tokens, cmd);
	}
	free_env(env);
	rl_clear_history();
//	clear_history(); // WARN: this function is not allowed by the subject, should we remove it? is it for running on mac()?
	return (0);
}
