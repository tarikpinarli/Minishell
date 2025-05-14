/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:30:46 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/14 16:31:29 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char **copy_env(char **envp)
{
	int i;
	int count;
	char **copy;

	i = 0;
	count = 0;
	while (envp[count])
		count++;
	copy = malloc(sizeof(char *) * (count + 1));
	if (!copy)
		return (NULL);
	while (i < count)
	{
		copy[i] = ft_strdup(envp[i]);
		i++;
	}
	copy[i] = NULL;
	return (copy);
}


static char	*build_cmd_path(char *dir, char *cmd)
{
	char	*tmp;
	char	*full;

	tmp = ft_strjoin(dir, "/");
	if (!tmp)
		return (NULL);
	full = ft_strjoin(tmp, cmd);
	free(tmp);
	if (!full)
		return (NULL);
	return (full);
}

char	*find_in_path(char *cmd)
{
	char	*path;
	char	**dirs;
	char	*full;
	int		i;

	path = getenv("PATH");
	i = 0;
	if (!path || !cmd)
		return (NULL);
	dirs = ft_split(path, ':');
	while (dirs[i])
	{
		full = build_cmd_path(dirs[i], cmd);
		if (!full)
			return (ft_free_split(dirs), NULL);
		if (access(full, X_OK) == 0)
		{
			ft_free_split(dirs);
			return (full);
		}
		free(full);
		i++;
	}
	ft_free_split(dirs);
	return (NULL);
}
