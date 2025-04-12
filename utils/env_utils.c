/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:30:46 by tpinarli          #+#    #+#             */
/*   Updated: 2025/04/12 17:52:21 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*find_in_path(char *cmd)
{
	char	*path = getenv("PATH");
	char	**dirs;
	char	*full;
	int		i = 0;

	if (!path || !cmd)
		return (NULL);

	dirs = ft_split(path, ':'); // libft'ten geliyor
	while (dirs[i])
	{
		full = malloc(strlen(dirs[i]) + strlen(cmd) + 2);
		if (!full)
			return (NULL);
		sprintf(full, "%s/%s", dirs[i], cmd); // full = dir/cmd

		if (access(full, X_OK) == 0)
		{
			// bulundu, döndür
			ft_free_split(dirs);
			return (full);
		}
		free(full);
		i++;
	}
	ft_free_split(dirs);
	return (NULL);
}