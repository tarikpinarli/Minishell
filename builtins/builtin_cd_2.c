/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd_2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 12:38:07 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/26 13:30:32 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*get_env_value(char **env, char *key)
{
	int	key_len;
	int	i;

	i = 0;
	key_len = ft_strlen(key);
	while (env[i])
	{
		if (ft_strncmp(env[i], key, key_len) == 0 && env[i][key_len] == '=')
			return (env[i] + key_len + 1);
		i++;
	}
	return (NULL);
}

char	*make_env_entry(const char *key, const char *value)
{
	size_t	key_len;
	size_t	total_len;
	char	*entry;

	key_len = ft_strlen(key);
	total_len = key_len + 1 + ft_strlen(value) + 1;
	entry = malloc(total_len);
	if (!entry)
		return (NULL);
	ft_strlcpy(entry, key, key_len + 1);
	ft_strlcat(entry, "=", key_len + 2);
	ft_strlcat(entry, value, total_len);
	return (entry);
}

void	append_env(char ***env, char *new_entry)
{
	int		i;
	char	**new_env;

	i = 0;
	while ((*env)[i])
		i++;
	new_env = malloc(sizeof(char *) * (i + 2));
	if (!new_env)
	{
		free(new_entry);
		return ;
	}
	i = 0;
	while ((*env)[i])
	{
		new_env[i] = (*env)[i];
		i++;
	}
	new_env[i] = new_entry;
	new_env[i + 1] = NULL;
	free(*env);
	*env = new_env;
}

void	update_env_var(char ***env, const char *key, const char *value)
{
	int		i;
	size_t	key_len;
	char	*new_entry;

	new_entry = make_env_entry(key, value);
	if (!new_entry)
		return ;
	key_len = ft_strlen(key);
	i = 0;
	while ((*env)[i])
	{
		if (!ft_strncmp((*env)[i], key, key_len) &&
			(*env)[i][key_len] == '=')
		{
			free((*env)[i]);
			(*env)[i] = new_entry;
			return ;
		}
		i++;
	}
	append_env(env, new_entry);
}
