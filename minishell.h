/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:27:45 by tpinarli          #+#    #+#             */
/*   Updated: 2025/04/28 16:39:06 by tpinarli         ###   ########.fr       */
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
# include <stdint.h> // necessary if we use size specific data types (such as int32_t, uint32_t and so forth)

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


/* WARN:
 * this could be a way to avoid having to go through the array every time to find
 * the NULL str.... which is sometimes quite complicated and risky.
 * by keeping track of the count, we would be more safe!
struct tokens
{
	size_t	count;
	t_token *tokens;
} t_tokens;
*/

typedef struct s_command
{
	char				**argv;      // Command and arguments
	t_redir				*in_redir;   // Input redirections
	t_redir				*out_redir;  // Output redirections
	struct s_command	*next;      // Next command in pipeline
}	t_command;



void	setup_signals(void);
void	handle_sigint(int sig);
void	handle_sigquit(int sig);

// tokenizer functions
int		tokenize(char *input, t_token **tokens);
//t_token	*tokenize(char *input); // older version? modified in order to avoid leaks
int		count_tokens(const char *str);
int	    ft_isspace(char c);
char	*ft_strncpy(char *dst, const char *src, size_t n);
// Parser functioons
t_command	*parse_tokens(t_token *tokens);
int			ft_strcmp(const char *s1, const char *s2);
uint32_t	expand_variables(t_token *tokens, int i);
// new from tarik2
void    expand_tokens(t_token *tokens, char *input, t_command *cmd);
// new from yonatan_3
t_token	*merge_tokens(t_token *tokens);
// exit code
int	last_exit_code(int set, int value);
//executor functioms
int		setup_redirections(t_command *cmd);
void	exec_command(t_command *cmd);
char	*find_in_path(char *cmd);
void	execute_pipeline(t_command *cmd);
// Free
void	ft_free_split(char **arr);
void	free_all(char *input, t_token *tokens, t_command *cmd);
// Builtin commands
int		is_builtin(char *cmd);
int		execute_builtin(t_command *cmd);
int		builtin_pwd(void);
//int builtin_cd(char **argv);
//int builtin_export(char **argv);
//int builtin_unset(char **argv);
//int builtin_env(void);
//int builtin_exit(char **argv);
//int builtin_echo(char **argv);

#endif
