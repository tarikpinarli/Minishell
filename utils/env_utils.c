/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:30:46 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/24 14:32:09 by tpinarli         ###   ########.fr       */
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
	copy = malloc(sizeof(char *) * (count + 1)); // WARN: is this really protected
	if (!copy)
		return (NULL);
	while (i < count)
	{
		copy[i] = ft_strdup(envp[i]); // WARN: unprotected malloc() failure
		i++;
	}
	copy[i] = NULL;
	return (copy);
}

static char	*build_cmd_path(char *dir, char *cmd)
{
	char	*tmp;
	char	*full;

	tmp = ft_strjoin(dir, "/"); // WARN: is this really protected
	if (!tmp)
		return (NULL);
	full = ft_strjoin(tmp, cmd); // WARN: is this really protected
	free(tmp);
	if (!full)
		return (NULL);
	return (full);
}

char	*find_in_path(char **env, char *cmd)
{
	char	*path;
	char	**dirs;
	char	*full;
	int		i;

	path = get_env_value(env ,"PATH");
	i = 0;
	if (!path || !cmd)
		return (NULL);
	dirs = ft_split(path, ':'); // WARN: malloc() failre unprotected here!
	while (dirs[i])
	{
		full = build_cmd_path(dirs[i], cmd);
		if (!full)
			return (free_two_dimensional_array(&dirs), NULL);
		if (access(full, X_OK) == 0)
		{
			free_two_dimensional_array(&dirs);
			return (full);
		}
		free(full);
		i++;
	}
	free_two_dimensional_array(&dirs);
	return (NULL);
}
