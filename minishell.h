/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:27:45 by tpinarli          #+#    #+#             */
/*   Updated: 2025/04/11 15:51:49 by tpinarli         ###   ########.fr       */
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


// Signal handling
extern int g_signal_status;

// Temel komut yapısı
typedef struct s_command
{
	char				**args;     // Argümanlar (örnek: ["echo", "merhaba"])
	char				*infile;    // < input dosyası
	char				*outfile;   // > veya >> dosyası
	int					append;     // 1 = >> (append), 0 = > (overwrite)
	struct s_command	*next;      // Pipe varsa, sonraki komut
}	t_command;

void	setup_signals(void);
void	handle_sigint(int sig);
void	handle_sigquit(int sig);

// tokenizer functions
char	**tokenize(const char *input);
int     count_tokens(const char *str);
int	    ft_isspace(char c);
char	*ft_strncpy(char *dst, const char *src, size_t n);
// Remove qoutes
char	*strip_quotes(const char *token);
// Parser functioons
t_command	*parse_tokens(char **tokens);
int	ft_strcmp(const char *s1, const char *s2);

#endif