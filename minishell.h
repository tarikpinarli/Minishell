/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:27:45 by tpinarli          #+#    #+#             */
/*   Updated: 2025/06/06 17:49:06 by tpinarli         ###   ########.fr       */
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
# include <sys/wait.h>
# include <sys/stat.h>
# include <errno.h>

# ifndef PATH_MAX
#  define PATH_MAX 4096
# endif

# define ALLOCATION_FAILURE "Fatal: memory allocation request has failed.\n\
Exiting this shell for precaution\n"

extern volatile sig_atomic_t	g_signal_status;

typedef enum e_loop_control
{
	BREAK,
	RESET,
	CONTINUE
}	t_loop_control;

typedef enum e_quote_type
{
	QUOTE_NONE,
	QUOTE_SINGLE,
	QUOTE_DOUBLE
}	t_quote_type;

typedef enum e_redir_type
{
	REDIR_NONE,		// (not a redirection)
	REDIR_IN,		// <
	REDIR_OUT,		// >
	REDIR_APPEND,	// >>
	REDIR_HEREDOC	// <<
}	t_redir_type;

typedef struct s_redir
{
	t_redir_type	type;
	char			*filename;
	int				is_heredoc_delimiter_quoted;
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

// signal handling
void		handle_sigint(int sig);
int			setup_signal_handling(uint32_t is_parent);
int			readline_signal_hook(void);
int			heredoc_signal_hook(void);
void		setup_signals(int *loop_control_flag);

// tokenizer functions
int			tokenize(char *input, t_token **tokens, char ***env);
int			count_tokens(const char *str);
int			fill_up_tokens(char *input, t_token **tokens, int count);
char		*ft_strncpy(char *dst, const char *src, size_t n);
int			ft_isspace(char c);

// parser functions
t_command	*parse_tokens(t_token *tokens, char *input, char ***env);
int			ft_strcmp(const char *s1, const char *s2);
void		merge_tokens(t_token *tokens, char *input, char ***env);

// expansion functions
void		expand_tokens(t_token *tokens, char *input, char ***env);
uint32_t	check_if_str_contains_vars_to_expand(char *string);
uint32_t	rebuild_expandable_string(char ***env, t_token *tokens, int i);
uint32_t	expand_last_exit_status(char **result, char **ptr);
uint32_t	expand_environment_variable(char ***env, char **ptr, char **result);
uint32_t	append_non_expandable_str(char **ptr, char **result);
uint32_t	strjoin_and_replace(char **s1, char **s2, uint8_t is_s2_heap);
uint32_t	rebuild_expandable_heredoc_line(char **line, char ***env);

// exit code
int			last_exit_code(int set, int value);

// executor functions
int			exec_single_command(t_command *cmd, char ***env);
int			find_in_path(char **env, char *cmd, char **path);
int			execute_pipeline(t_command *cmd, char ***env);
void		handle_execve_error(char *str, char *path, t_command **cmd, char ***env);
int			setup_pipe(int *pipefd);
void		prepare_child(t_command *cmd, int prev_fd, int *pipefd);
void		update_prev_fd(t_command *cmd, int *prev_fd, int *pipefd);
int			wait_for_children(pid_t pid, size_t n_of_children);
void		check_if_directory(char **path, t_command **cmd, char ***env);

// Redirection functions
int			setup_redirections(t_command *cmd);
int			run_heredocs(t_command **current, char ***env, t_command **head);

// free
void		free_cmd(t_command **cmd);
void		free_tokens_and_input(t_token **tokens, char **input);
void		free_all(char **input, t_token **tokens, t_command **cmd);
void		free_two_dimensional_array(char ***arr);
void		free_deprecated_strings(t_token *tokens, size_t k);
void		free_tokens(t_token **tokens);
int			cleanup_heredocs(t_redir *current_in_redir);
void		free_rest(char **path, t_command **cmd, char ***env);
void		cleaning_after_exec(char *input, t_token *tokens, t_command *cmd);
void		final_cleaning(char **env);

// builtin commands
void		copy_env(char **envp, char ***env_copy);
int			is_builtin(char *cmd);
int			execute_builtin(t_command *cmd, int pid_flag, char ***env);
int			builtin_pwd(char **argv);
int			builtin_unset(char **argv, char ***env);
int			builtin_env(char ***env);
int			builtin_exit(char **argv, t_command *cmd, int pid_flag, char ***env);
int			builtin_echo(char **argv);
char		*get_env_value(char **env, char *key);

// builtin_cd
int			builtin_cd(char **argv, char ***env);
int			update_env_var(char ***env, const char *key, const char *value);
int			update_pwd_vars(char ***env, const char *oldpwd, const char *newpwd);
// builtin_export
int			builtin_export(char **argv, char ***env);
int			valid_identifier(char *str);
void		sort_and_print_env(char **env);
// builtin export and unset utils
int			var_exist(char *arg, char **env);
int			remove_env_var(char ***env, int index);
// helpers
void		mute_args(int argc, char **argv);

#endif
