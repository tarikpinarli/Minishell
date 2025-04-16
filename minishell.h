/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:27:45 by tpinarli          #+#    #+#             */
/*   Updated: 2025/04/16 11:29:25 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

#include "libft/libft.h"
# include <signal.h>
# include <stdio.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>


// Signal handling
extern int g_signal_status;

// Temel komut yapısı

typedef struct s_token
{
	char			*str;
	t_quote_type	quote;
}	t_token;

typedef enum e_quote_type
{
	QUOTE_NONE,
	QUOTE_SINGLE,
	QUOTE_DOUBLE
}	t_quote_type;

typedef struct s_command
{
	char				**argv;      // Command and arguments
	t_redir				*in_redir;   // Input redirections
	t_redir				*out_redir;  // Output redirections
	struct s_command	*next;      // Next command in pipeline
}	t_command;

typedef struct s_redir
{
	t_redir_type	type;
	char			*filename;
	struct s_redir	*next;
}	t_redir;

typedef enum e_redir_type
{
	REDIR_IN,     // <
	REDIR_OUT,    // >
	REDIR_APPEND, // >>
	REDIR_HEREDOC // <<
}	t_redir_type;

void	setup_signals(void);
void	handle_sigint(int sig);
void	handle_sigquit(int sig);

// tokenizer functions
t_token	*tokenize(char *input);
int     count_tokens(const char *str);
int	    ft_isspace(char c);
char	*ft_strncpy(char *dst, const char *src, size_t n);
// Remove qoutes
char	*strip_quotes(const char *token);
// Parser functioons
t_command	*parse_tokens(t_token *tokens);
int	ft_strcmp(const char *s1, const char *s2);
char	*expand_variables(const char *str);
// exit code
int	last_exit_code(int set, int value);
//executor functioms
int	setup_redirections(t_command *cmd);
void	exec_command(t_command *cmd);
char	*find_in_path(char *cmd);
void	execute_pipeline(t_command *cmd);
// Free
void	ft_free_split(char **arr);
#endif