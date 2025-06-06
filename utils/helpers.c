/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpers.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/05 19:22:12 by tpinarli          #+#    #+#             */
/*   Updated: 2025/06/05 20:03:32 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	mute_args(int argc, char **argv)
{
	if (argc > 1)
	{
		(void)argv;
		write(2, "Too many arguments\n", sizeof("Too many arguments\n") - 1);
		// printf("minishell: %s: No such file or directory\n", argv[1]);
		exit (last_exit_code(0, 0));
	}
}
