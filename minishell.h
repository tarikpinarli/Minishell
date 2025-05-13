/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:27:45 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/12 19:38:24 by ykadosh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft/libft.h"
# include <signal.h>
# include <stdio.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>
# include <stdint.h>

# ifndef PATH_MAX
#  define PATH_MAX 4096
# endif

// Signal handling
extern int g_signal_status;

typedef enum e_quote_type
{
	QUOTE_NONE,
	QUOTE_SINGLE,
	QUOTE_DOUBLE
}	t_quote_type;

typedef enum e_redir_type
{
	REDIR_IN,     // <
	REDIR_OUT,    // >
	REDIR_APPEND, // >>
	REDIR_HEREDOC // <<
}	t_redir_type;

typedef struct s_redir
{
	t_redir_type	type;
	char			*filename;
	struct s_redir	*next;
}	t_redir;

typedef struct s_token
{
	char			*str;
	t_quote_type	quote;
	int				line_id;
}	t_token;

typedef struct s_command
{
	char				**argv;		// Command and arguments
	t_redir				*in_redir;	// Input redirections
	t_redir				*out_redir;	// Output redirections
	struct s_command	*next;		// Next command in pipeline
}	t_command;


// signal functions
void		setup_signals(void);
void		handle_sigint(int sig);
void		handle_sigquit(int sig);

// tokenizer functions
int			tokenize(char *input, t_token **tokens);
int			count_tokens(const char *str);
int			fill_up_tokens(char *input, t_token **tokens, int count);
char		*ft_strncpy(char *dst, const char *src, size_t n);
int			ft_isspace(char c);

// parser functions
t_command	*parse_tokens(t_token *tokens);
int			ft_strcmp(const char *s1, const char *s2);
void		merge_tokens(t_token *tokens, char *input);

// expansion functions
void		expand_tokens(t_token *tokens, char *input);
uint32_t	rebuild_expandable_string(t_token *tokens, int i);
uint32_t	strjoin_and_replace(char **s1, char **s2, uint8_t is_s2_heap);
uint32_t	handle_empty_expansion(t_token *tokens, int i, char **ptr);

// exit code
int			last_exit_code(int set, int value);

// executor functions
int			setup_redirections(t_command *cmd);
void		exec_command(t_command *cmd, char ***env);
char		*find_in_path(char *cmd);
void		execute_pipeline(t_command *cmd, char ***env);

// free
void		free_input(char *input);
void		free_tokens(t_token *tokens);
void		free_cmd(t_command *cmd);
void		free_all(char *input, t_token *tokens, t_command *cmd);
void		ft_free_split(char **arr);
void		free_deprecated_strings(t_token *tokens, size_t k);

// NOTE: Question to Tarik: Do you think we should consider changing the variable
// name of "argv" that is used for the builtins, because there is already one
// argv variable in the main? Or is it the same one?
// builtin commands
int			is_builtin(char *cmd);
int			execute_builtin(t_command *cmd, int pid_flag, char ***env);
int			builtin_pwd(void);
//int		builtin_cd(char **argv);
int			builtin_export(char **argv, int pid_flag, char ***env);
//int		builtin_unset(char **argv);
int			builtin_env(char ***env);
int			builtin_exit(char **argv, t_command *cmd, int pid_flag);
int			builtin_echo(char **argv);

// debug functions // WARN: remove before evaluation if just for debugging
void		print_command(t_command *cmd);

#endif
