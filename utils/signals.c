/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:30:51 by tpinarli          #+#    #+#             */
/*   Updated: 2025/04/11 11:35:39 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	g_signal_status = 0;


void	handle_sigint(int sig)
{
	(void)sig;
	g_signal_status = 130;
	rl_replace_line("", 0);
	write(1, "\n", 1);
	rl_on_new_line();
	rl_redisplay();
}

void	handle_sigquit(int sig)
{
	(void)sig;
	g_signal_status = 131;
	// Do nothing (as required by the subject)
}

void	setup_signals(void)
{
    
	signal(SIGINT, handle_sigint);
	signal(SIGQUIT, handle_sigquit);
}
