/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:27:45 by tpinarli          #+#    #+#             */
/*   Updated: 2025/04/11 15:17:22 by tpinarli         ###   ########.fr       */
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

void	setup_signals(void);
void	handle_sigint(int sig);
void	handle_sigquit(int sig);

// tokenizer functions
char	**tokenize(const char *input);
int     count_tokens(const char *str);
int	    ft_isspace(char c);
char	*ft_strncpy(char *dst, const char *src, size_t n);
// Parcer functions
char	*strip_quotes(const char *token);

#endif