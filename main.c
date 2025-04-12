/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:27:52 by tpinarli          #+#    #+#             */
/*   Updated: 2025/04/12 18:31:44 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int main(void)
{
	char		*input;
	t_token		*tokens;
	t_command	*cmd;

	rl_catch_signals = 0;
	setup_signals();

	while (1)
	{
		input = readline("minishell$ ");
		if (!input)
		{
			printf("exit\n");
			break;
		}
		if (*input)
			add_history(input);

		tokens = tokenize(input);
		if (!tokens)
		{
			free(input);
			continue;
		}
		cmd = parse_tokens(tokens);
        if (cmd && cmd->next) // pipe varsa
            execute_pipeline(cmd);
        else
            exec_command(cmd); // tek komutsa eski sistem


		// Memory cleanup
		for (int i = 0; tokens[i].str; i++)
			free(tokens[i].str);
		free(tokens);
		free(input);
		for (int i = 0; cmd->argv && cmd->argv[i]; i++)
			free(cmd->argv[i]);
		free(cmd->argv);
		while (cmd->in_redir)
		{
			t_redir *tmp = cmd->in_redir;
			cmd->in_redir = cmd->in_redir->next;
			free(tmp->filename);
			free(tmp);
		}
		while (cmd->out_redir)
		{
			t_redir *tmp = cmd->out_redir;
			cmd->out_redir = cmd->out_redir->next;
			free(tmp->filename);
			free(tmp);
		}
		free(cmd);
	}
	rl_clear_history();
	return 0;
}

