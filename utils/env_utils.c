/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:30:46 by tpinarli          #+#    #+#             */
/*   Updated: 2025/06/02 19:35:19 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
// alternate version
void	copy_env(char **envp, char ***env_copy)
{
	int	i;
	int	count;

	i = 0;
	count = 0;
	while (envp[count])
		count++;
	*env_copy = malloc(sizeof(char *) * (count + 1)); // Protected!
	if (!*env_copy)
	{
		write(2, ALLOCATION_FAILURE, sizeof(ALLOCATION_FAILURE) -1);
		exit(1);
	}
	while (i < count)
	{
		*env_copy[i] = ft_strdup(envp[i]); // Protected!
		if (!(*env_copy)[i])
		{
			free_two_dimensional_array(env_copy);
			write(2, ALLOCATION_FAILURE, sizeof(ALLOCATION_FAILURE) -1);
			exit(1);
		}
		i++;
	}
	(*env_copy)[i] = NULL;
}
*/

char **copy_env(char **envp)
{
	int i;
	int count;
	char **copy;

	i = 0;
	count = 0;
	while (envp[count])
		count++;
	copy = malloc(sizeof(char *) * (count + 1)); // Protected!
	if (!copy)
		return (NULL);
	while (i < count)
	{
		copy[i] = ft_strdup(envp[i]); // Protected!
		if (!copy[i])
		{
			free_two_dimensional_array(&copy);
			return (NULL);
		}
		i++;
	}
	copy[i] = NULL;
	return (copy);
}

static char	*build_cmd_path(char *dir, char *cmd)
{
	char	*tmp;
	char	*full;

	(void)cmd;
	tmp = ft_strjoin(dir, "/"); // Protected!
	if (!tmp)
		return (NULL);
	full = ft_strjoin(tmp, cmd); // Protected!
	free(tmp);
	if (!full)
		return (NULL);
	return (full);
}

int	find_in_path(char **env, char *cmd, char **path)
{
	char	*get_path;
	char	**dirs;
	int		i;

	get_path = get_env_value(env ,"PATH", sizeof("PATH") - 1);
	i = 0;
	if (!get_path || !cmd)
		return (-2);
	dirs = ft_split(get_path, ':'); // WARN: This is the spot where we were checking the memory leaks with a forced dird = NULL; ("ls | a")
	if (!dirs)
		return (-1);
	while (dirs[i])
	{
		*path = build_cmd_path(dirs[i], cmd);
		if (!*path)
			return (free_two_dimensional_array(&dirs), -1);
		if (access(*path, X_OK) == 0)
			break;
		free(*path);
		*path = NULL;
		i++;
	}
	free_two_dimensional_array(&dirs);
	return (0);
}
