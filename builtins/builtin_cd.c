/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpinarli <tpinarli@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 10:30:22 by tpinarli          #+#    #+#             */
/*   Updated: 2025/05/21 16:11:05 by tpinarli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"


static char	*make_env_entry(const char *key, const char *value)
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

static void	append_env(char ***env, char *new_entry)
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
		if (ft_strncmp((*env)[i], key, key_len) == 0 &&
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



void	update_pwd_vars(char ***env, const char *oldpwd, const char *newpwd)
{
	update_env_var(env, "OLDPWD", oldpwd);
	update_env_var(env, "PWD", newpwd);
}


int builtin_cd(char **argv, char ***env)
{
    char oldpwd[PATH_MAX];
    char newpwd[PATH_MAX];

    if (!argv || !argv[0])
        return (0);
    if (!argv[1])
        return(ft_putendl_fd("cd: HOME/USER not set", 2), 1);
    if (argv[2])
        return(ft_putstr_fd("minishell: ", 2),
        ft_putstr_fd(argv[0], 2),
        ft_putendl_fd(": too many arguments", 2), 1);
    if (!getcwd(oldpwd, sizeof(oldpwd)))
        return (1);
    if (chdir(argv[1]) != 0)
    {
        perror("cd");
        return (1);
    }
    if (getcwd(newpwd, sizeof(newpwd)))
        update_pwd_vars(env, oldpwd, newpwd);
    return (0);
}
