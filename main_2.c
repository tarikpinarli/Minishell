/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 19:51:05 by ykadosh           #+#    #+#             */
/*   Updated: 2025/05/07 15:26:21 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "minishell.h"

/*
* return values:
* ◦ 0: if everything ran smoothly
* ◦ 1: upon failure of request for dynamic memory allocation
*/
int main(void)
{
	char		*input;
	t_token		*tokens;
	t_command	*cmd;

	// FIXME:
	rl_catch_signals = 0; // Disabling read_line()s default signal handling. It violates the subject;
	// but it resolves the issue of SIGQUIT signal Ctrl+\, which is not supposed to do anything, but
	// now it prints that and brings the cursor back to the start of the line...
	setup_signals();
	while (1)
	{
		input = NULL;
		tokens = NULL;
		cmd = NULL;
		input = readline("minishell$ ");
		if (!input)
		{
			free_all(input, tokens, cmd);
			printf("exit\n");
			break;
		}
		if (input[0])
			add_history(input);
		else  // TODO: Ask Tariq how does this else block happen? since we already check for !input & input[0] beforehand.
		{
			free(input);
			continue;
		}
		if (tokenize(input, &tokens) < 0) // tokenize frees input for us if (count < 0), and nothing else gets allocated in that case.
			continue;
		if (!tokens)
		{
			free(input);
			return (1);
		}


		// WARN: This is just printf() debugging, to easily follow the stages of the tokenizing process:
		/*size_t	j;

		j = 0;
		printf("\nin MAIN(), after tokenize(), the strings are:\n\t");
		while (tokens[j].str)
		{
			printf("tokens[%zu].str:	<%s>\n\t", j, tokens[j].str);
			j++;
		}
		printf("\n\n\n");*/

		expand_tokens(tokens, input, cmd);

		/*j = 0;
		printf("in MAIN, after expand_tokens(), the strings are:\n\t");
		while (tokens[j].str)
		{
			printf("tokens[%zu].str:	<%s>\n\t", j, tokens[j].str);
			j++;
		}
		printf("\n\n\n");*/

		tokens = merge_tokens(tokens);
		if (!tokens)
		{
			free(input);
			continue;
		}
		cmd = parse_tokens(tokens);
		if (!cmd)
		{
			free_all(input, tokens, cmd);
			printf("Parcing failed.\n");
			continue;
		}
		/*int	i = 0;
		while (tokens[i].str)
		{
			printf("token[%d]: <%s>	---- quote type:	<%d>	---- line_id:	<%d>	--- length:	<%zu>\n", i, tokens[i].str, tokens[i].quote, tokens[i].line_id, ft_strlen(tokens[i].str));
			i++;
		}*/
		
		//if (is_builtin(cmd->arg[0]))
			// if its a builtin command execute.
        if (cmd && cmd->next) // If there is pipe cmd->next exists
            execute_pipeline(cmd);
        else if (cmd)
            exec_command(cmd); // If its a single command*/
		free_all(input, tokens, cmd);
	}
	rl_clear_history();
//	clear_history(); // WARN: this function is not allowed by the subject, should we remove it? is it for running on mac()?
	return 0;
}
